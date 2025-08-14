#include "TUIManager.h"
#include "TPlayerUIWidget.h"
#include "Character/TCharacterBase.h"
#include "Item/TWeaponBase.h"
#include "Character/TPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Area/TCapturePoint.h"
#include "EngineUtils.h"
#include "Character/TNonPlayerCharacter.h"
#include "Character/TNonPlayerCharacterSword.h"
#include "Spawner/TEnemySpawner.h"
#include "Engine/DamageEvents.h"
#include "TGameMode.h"
#include "Engine/Engine.h"
#include "UnifiedBuffer.h"


void UTUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	//테스트 로그
	UE_LOG(LogTemp,Warning,TEXT("UIManager Initialized!!"));
}

void UTUIManager::Deinitialize()
{
	//타이머 세팅
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(UIUpdateTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(MonsterMonitorTimer);
		GetWorld()->GetTimerManager().ClearTimer(HitDetectionTimer);
		
	}
	Super::Deinitialize();
}

void UTUIManager::CreatePlayerUI()
{
	// PlayerUIWidgetClass가 설정되지 않았으면 직접 로드
	if (!PlayerUIWidgetClass)
	{
		// Team02/UI/InGame/WBP_PlayerUI 경로로 로드
		PlayerUIWidgetClass = LoadClass<UTPlayerUIWidget>(nullptr, TEXT("/Game/Team02/UI/InGame/WBP_PlayerUI.WBP_PlayerUI_C"));
        
		if (!PlayerUIWidgetClass)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load WBP_PlayerUI! Check the path."));
			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("WBP_PlayerUI loaded successfully!"));
		}
	}

	if (GetWorld())
	{

		// ⭐ 여기서 GameMode 참조 설정
		GameModeRef = Cast<ATGameMode>(GetWorld()->GetAuthGameMode());
		if (GameModeRef)
		{
			UE_LOG(LogTemp, Warning, TEXT("✅ GameMode reference set successfully!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Failed to get GameMode reference in CreatePlayerUI!"));
		}
		
		
		PlayerUIWidget = CreateWidget<UTPlayerUIWidget>(GetWorld(), PlayerUIWidgetClass);
		if (PlayerUIWidget)
		{
			PlayerUIWidget->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("Player UI Created Successfully!"));
			
			// UI 업데이트 타이머
			GetWorld()->GetTimerManager().SetTimer(
				UIUpdateTimerHandle,
				this,
				&UTUIManager::UpdateAllUI,
				0.1f,
				true);

			//히트 감지 시작
			StartHitDetection();
			
			// 거점 자동 검색 및 등록(UI 생성 이후)
			FindAndRegisterCapturePoints();

			// 스포너 및 몬스터 모너터링 시작
			FindAndRegisterEnemySpawners();
			StartMonitoringMonsters();

			

			//초기 미션 설정
			CurrentMissionObjective=TEXT("Mission:");
			PlayerUIWidget->UpdateMissionObjective(TEXT("Mission:"));

			// 첫 임무 시작시 1.5초 지연
			FTimerHandle InitMissionTimer;
			GetWorld()->GetTimerManager().SetTimer(
				InitMissionTimer,
				[this]()
				{
					UpdateMissionState();
				},
				1.5f,
				false
				);
			
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create PlayerUIWidget!"));
		}
	}
}



void UTUIManager::SetPlayerCharacter(ATCharacterBase* PlayerChar)
{
	PlayerCharacter=PlayerChar;

	// find player's current weapon
	if (ATPlayerCharacter*PC=Cast<ATPlayerCharacter>(PlayerChar))
	{
		CurrentWeapon=PC->CurrentWeapon;
	}
}

void UTUIManager::UpdatePlayerHP()
{
	//활성화된 플레이어 캐릭터 가져오기
	if (APlayerController* PC=GetWorld()->GetFirstPlayerController())
	{
		if (APawn* CurrentPawn=PC->GetPawn())
		{
			if (ATPlayerCharacter*CurrentPlayer=Cast<ATPlayerCharacter>(CurrentPawn))
			{
				float CurrentHP=CurrentPlayer->GetCurrentHP();
				float MaxHP=CurrentPlayer->GetMaxHP();
				PlayerUIWidget->UpdateHPBar(CurrentHP,MaxHP);

				return;
			}
		}
	}
	// 플레이어를 찾지 못했을 때
	UE_LOG(LogTemp, Error, TEXT("❌ UpdatePlayerHP: No valid player found"));
}

void UTUIManager::UpdatePlayerAmmo()
{
	if (PlayerUIWidget && CurrentWeapon)
	{
		PlayerUIWidget->UpdateAmmoInfo(CurrentWeapon->GetCurrentAmmo(),CurrentWeapon->GetTotalAmmo());
	}
}

void UTUIManager::UpdateWeaponInfo()
{
	// 무기 변경 감지
	if (CurrentWeapon != PreviousWeapon)
	{
		if (CurrentWeapon && PlayerUIWidget)
		{
			// 무기이름 가쟈오기
			FString WeaponName=CurrentWeapon->GetWeaponTypeString();
			PlayerUIWidget->UpdateWeaponName(WeaponName);

			UE_LOG(LogTemp,Warning,TEXT("Weapon changed to: %s"), *WeaponName);

			//첫번째 무기 습득감지
			if (bFirstCaptureCompleted && !bWeaponPickedUp)
			{
				// 새로운 무기인지 확인(기본 무기가 아닌 경우)
				if (WeaponName != TEXT("Pistol") && WeaponName !=TEXT("No Weapon"))
				{
					bWeaponPickedUp=true;
					bWeaponUnlocked=true;
					MoveToNextCapturePoint(); // 2번쨰 거점으로 이동

					UE_LOG(LogTemp, Warning, TEXT("🔫 New weapon '%s' picked up! Moving to next objective"), *WeaponName);
					UpdateMissionState(); // 미션 업데이트
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("🔫 Basic weapon detected: %s (not counting as new weapon)"), *WeaponName);
				}
			}
			//두번쨰 무기 습득 감지(2거점 가기 전 라이플)
			else if (bWeaponPickedUp && !bSecondWeaponPickedUp && CurrentCaptureIndex==1)
			{
				//라이플이나 새로운 무기인지 확인
				if (WeaponName != TEXT("Pistol") && WeaponName !=TEXT("No Weapon"))
				{
					//이전에 먹은 무기와 다른 무기인지 확인
					if (PreviousWeapon && WeaponName != PreviousWeapon->GetWeaponTypeString())
					{
						bSecondWeaponPickedUp=true;
						UE_LOG(LogTemp, Warning, TEXT("🔫 Second weapon '%s' picked up!"), *WeaponName);
						UpdateMissionState();// 미션 업데이트
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("🔫 Weapon change detected but conditions not met:"));
				UE_LOG(LogTemp, Warning, TEXT("  bFirstCaptureCompleted: %s"), bFirstCaptureCompleted ? TEXT("YES") : TEXT("NO"));
				UE_LOG(LogTemp, Warning, TEXT("  bWeaponPickedUp: %s"), bWeaponPickedUp ? TEXT("YES") : TEXT("NO"));
			}
		}
		
		else if (!CurrentWeapon && PlayerUIWidget)
		{
			//무기가 없을때
			PlayerUIWidget->UpdateWeaponName(TEXT("No Weapon"));
		}

		// 이전 무기 업데이트
		PreviousWeapon=CurrentWeapon;
	}
}


void UTUIManager::ShowCaptureUI(const FString& AreaName)
{
	if (PlayerUIWidget)
	{
		PlayerUIWidget->ShowCaptureUI(AreaName);
	}
}

void UTUIManager::HideCaptureUI()
{
	if (PlayerUIWidget)
	{
		PlayerUIWidget->HideCaptureUI();
	}
}

void UTUIManager::UpdateCaptureProgress(float Progress)
{
	if (PlayerUIWidget)
	{
		PlayerUIWidget->UpdateCaptureProgress(Progress);
	}
}

void UTUIManager::RegisterCapturePoint(class ATCapturePoint* CapturePoint)
{
	if (CapturePoint)
	{
		CurrentCapturePoint=CapturePoint;
	}
}

void UTUIManager::FindAndRegisterCapturePoints()
{
    if (UWorld* World = GetWorld())
    {
        AllCapturePoints.Empty();
        ATCapturePoint* FirstPoint = nullptr;
        ATCapturePoint* SecondPoint = nullptr;

        UE_LOG(LogTemp, Warning, TEXT("Searching for capture points by name..."));

        // 모든 거점 찾기
        for (TActorIterator<ATCapturePoint> ActorItr(World); ActorItr; ++ActorItr)
        {
            ATCapturePoint* CapturePoint = *ActorItr;
            if (CapturePoint)
            {
                FString ActorName = CapturePoint->GetName();
                UE_LOG(LogTemp, Warning, TEXT("Found CapturePoint: %s"), *ActorName);
                
                // ⭐ 이름으로 1거점/2거점 구분
                if (ActorName.Contains(TEXT("First")) || 
                    ActorName.Contains(TEXT("1")) || 
                    ActorName.Contains(TEXT("One")))
                {
                    FirstPoint = CapturePoint;
                    UE_LOG(LogTemp, Warning, TEXT("Identified as FIRST capture point: %s"), *ActorName);
                }
                else if (ActorName.Contains(TEXT("Second")) || 
                         ActorName.Contains(TEXT("2")) || 
                         ActorName.Contains(TEXT("Two")))
                {
                    SecondPoint = CapturePoint;
                    UE_LOG(LogTemp, Warning, TEXT("Identified as SECOND capture point: %s"), *ActorName);
                }
                else
                {
                    // 이름에 식별자가 없는 경우 경고
                    UE_LOG(LogTemp, Warning, TEXT("CapturePoint name doesn't contain identifier: %s"), *ActorName);
                }
            }
        }

        // ⭐ 순서대로 배열에 추가 (1거점 먼저, 2거점 나중에)
        if (FirstPoint)
        {
            AllCapturePoints.Add(FirstPoint);
            UE_LOG(LogTemp, Warning, TEXT("Added First Point to index 0: %s"), *FirstPoint->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("First capture point not found! Check actor names."));
        }

        if (SecondPoint)
        {
            AllCapturePoints.Add(SecondPoint);
            UE_LOG(LogTemp, Warning, TEXT("Added Second Point to index 1: %s"), *SecondPoint->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Second capture point not found! Check actor names."));
        }

        // 결과 확인
        if (AllCapturePoints.Num() >= 2)
        {
            // 첫 번째 거점을 현재 거점으로 설정
            CurrentCapturePoint = AllCapturePoints[CurrentCaptureIndex];
            CapturePointName = CurrentCapturePoint->GetName();

            UE_LOG(LogTemp, Warning, TEXT("✅ Successfully set up %d capture points:"), AllCapturePoints.Num());
            UE_LOG(LogTemp, Warning, TEXT("   1st Point (Index 0): %s"), *AllCapturePoints[0]->GetName());
            UE_LOG(LogTemp, Warning, TEXT("   2nd Point (Index 1): %s"), *AllCapturePoints[1]->GetName());
            UE_LOG(LogTemp, Warning, TEXT("   Starting with: %s"), *CapturePointName);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("❌ Need exactly 2 capture points! Found: %d"), AllCapturePoints.Num());
            UE_LOG(LogTemp, Error, TEXT("   Make sure actor names contain 'First'/'1' and 'Second'/'2'"));
        }
    }
}
// 임무 관련 함수
void UTUIManager::SetMissionObjective(const FString& NewObjective)
{
	CurrentMissionObjective=NewObjective;

	if (PlayerUIWidget)
	{
		PlayerUIWidget->UpdateMissionObjective(NewObjective);
		UE_LOG(LogTemp,Warning,TEXT("Mission Objective set to: %s"), *NewObjective);
		
	}
}

void UTUIManager::UpdateMissionProgress()
{
	UpdateMissionState();
}

// TUIManager.cpp의 UpdateMissionState() 함수를 이것으로 교체하세요

void UTUIManager::UpdateMissionState()
{
    FString NewObjective;
    
    // 임무 확인 로그
    UE_LOG(LogTemp, Warning, TEXT("🎯 Mission State Update:"));
    UE_LOG(LogTemp, Warning, TEXT("  bSecondCaptureCompleted: %s"), bSecondCaptureCompleted ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("  bFirstCaptureCompleted: %s"), bFirstCaptureCompleted ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("  bWeaponPickedUp: %s"), bWeaponPickedUp ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("  bSecondWeaponPickedUp: %s"), bSecondWeaponPickedUp ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("  bWaveCompleted: %s"), bWaveCompleted ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("  bWaveActive: %s"), bWaveActive ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("  CurrentCaptureIndex: %d"), CurrentCaptureIndex);

    // 게임 완료
    if (bSecondCaptureCompleted)
    {
        NewObjective = TEXT("Victory!");  // 짧게 수정
        //승리 이벤트 발생
        OnVictoryEvent.Broadcast();
        UE_LOG(LogTemp, Warning, TEXT("Game Completed! All Capture points secured!!"));
    }
    // 2거점 점령 관련 (두 번째 무기 습득 완료 후)
    else if (bSecondWeaponPickedUp && CurrentCaptureIndex == 1)
    {
        if (bNearCapturePoint && bCapturePhase)
        {
            NewObjective = TEXT("Capture Point 2");  // 짧게 수정
        }
        else if (bNearCapturePoint && !bCapturePhase)
        {
            NewObjective = TEXT("Enter Point 2");  // 짧게 수정
            bCapturePhase = true;
        }
        else
        {
            NewObjective = TEXT("Move to Point 2");  // 짧게 수정
        }
    }
    // 두 번째 무기 습득 단계 (첫 번째 무기 습득 후, 2거점 가기 전)
    else if (bWeaponPickedUp && !bSecondWeaponPickedUp && CurrentCaptureIndex == 1)
    {
        NewObjective = TEXT("Get Rifle");  // 새로 추가
    }
    // 첫 번째 무기 습득 단계 (1거점 완료 후)
    else if (bFirstCaptureCompleted && !bWeaponPickedUp)
    {
        NewObjective = TEXT("Get Shotgun");  // 짧게 수정
    }
    // 1거점 점령 관련
    else if (bWaveCompleted && CurrentCaptureIndex == 0)
    {
        if (bNearCapturePoint && bCapturePhase)
        {
            NewObjective = TEXT("Capture Point 1");  // 짧게 수정
        }
        else if (bNearCapturePoint && !bCapturePhase)
        {
            NewObjective = TEXT("Enter Point 1");  // 짧게 수정
            bCapturePhase = true;
        }
        else
        {
            NewObjective = TEXT("Move to Point 1");  // 짧게 수정
        }
    }
    //몬스터 처치 관련
    else if (bWaveActive || TrackedMonsters.Num() > 0)
    {
        int32 SpawnerBasedMax = 0;
        for (ATEnemySpawner* Spawner : RegisteredSpawners)
        {
            if (Spawner)
            {
                SpawnerBasedMax += Spawner->MaxSpawnCount;
            }
        }

        int32 RemainingCount = FMath::Max(0, SpawnerBasedMax - MonsterKillCount);
        NewObjective = FString::Printf(TEXT("Kill Enemies (%d left)"), RemainingCount);  // 짧게 수정
    }
    else
    {
        NewObjective = TEXT("Kill All Enemies");  // 짧게 수정
    }
    
    // 미션 목표가 실제로 변경되었을 때만 업데이트
    if (CurrentMissionObjective != NewObjective)
    {
        UE_LOG(LogTemp, Warning, TEXT("🔄 Mission Updated: '%s' -> '%s'"), 
               *CurrentMissionObjective, *NewObjective);
        SetMissionObjective(NewObjective);
    }
}

void UTUIManager::FindAndRegisterEnemySpawners()
{
	if (UWorld* World=GetWorld())
	{
		RegisteredSpawners.Empty();

		UE_LOG(LogTemp,Warning,TEXT("Searching for enemy spawners..."));
		// 월드에서 모든 스포너 찾기
		for (TActorIterator<ATEnemySpawner> ActorItr(World); ActorItr; ++ActorItr)
		{
			ATEnemySpawner* Spawner= *ActorItr;
			if (Spawner)
			{
				RegisteredSpawners.Add(Spawner);
				UE_LOG(LogTemp,Warning,TEXT("Found spawner: %s (MaxSpawn:%d)"),
					*Spawner->GetName(),Spawner->MaxSpawnCount);
			}
		}
		if (RegisteredSpawners.Num()>0)
		{
			UE_LOG(LogTemp,Warning,TEXT("Total %d spawners registered"),RegisteredSpawners.Num());
			UpdateWaveInfoFromSpawners();
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("No enemy spawners found!"));
		}
	}
}

void UTUIManager::StartMonitoringMonsters()
{
	if (GetWorld())
	{
		//0.5초마다 몬스터 상태 체크
		GetWorld()->GetTimerManager().SetTimer(
			MonsterMonitorTimer,
			this,
			&UTUIManager::UpdateMonsterStatus,
			0.5f,
			true);

		UE_LOG(LogTemp,Warning,TEXT("Monster monitoring started!"));
	}
}

void UTUIManager::StopMonitoringMonsters()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(MonsterMonitorTimer);
		UE_LOG(LogTemp, Warning, TEXT("Monster monitoring stopped"));
	}
}

void UTUIManager::UpdateMonsterStatus()
{
	FindAllMonstersInWorld();
	
    int32 CurrentMonsterCount = TrackedMonsters.Num();

    // 🔍 강화된 디버그 로그
    UE_LOG(LogTemp, Warning, TEXT("🔍 Monster Status Check:"));
    UE_LOG(LogTemp, Warning, TEXT("  Tracked Monsters: %d"), CurrentMonsterCount);
    UE_LOG(LogTemp, Warning, TEXT("  MonsterKillCount: %d"), MonsterKillCount);
    UE_LOG(LogTemp, Warning, TEXT("  bWaveActive: %s"), bWaveActive ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("  bWaveCompleted: %s"), bWaveCompleted ? TEXT("YES") : TEXT("NO"));

    // 🔍 GameMode 웨이브 상태 확인
    bool bGameModeWaveActive = false;
    if (GameModeRef)
    {
        bGameModeWaveActive = GameModeRef->bIsWaveActive;
        UE_LOG(LogTemp, Warning, TEXT("  GameMode Wave Active: %s"), bGameModeWaveActive ? TEXT("YES") : TEXT("NO"));
    }

    // 🔍 스포너 상태 상세 체크
    bool bAnySpawnerActive = false;
    int32 TotalSpawnersCompleted = 0;
    int32 SpawnerBasedMax = 0;
    
    for (int32 i = 0; i < RegisteredSpawners.Num(); i++)
    {
        ATEnemySpawner* Spawner = RegisteredSpawners[i];
        if (Spawner)
        {
            int32 CurrentSpawned = Spawner->GetCurrentSpawned();
            int32 MaxSpawn = Spawner->MaxSpawnCount;
            SpawnerBasedMax += MaxSpawn;
            
            bool bSpawnerCompleted = (CurrentSpawned >= MaxSpawn);
            
            if (!bSpawnerCompleted && CurrentSpawned > 0)
            {
                bAnySpawnerActive = true;
            }
            else if (bSpawnerCompleted)
            {
                TotalSpawnersCompleted++;
            }
        }
    }

   
    if (!bWaveActive && !bWaveCompleted)
    {

    	UE_LOG(LogTemp, Warning, TEXT("🔍 Checking wave start conditions:"));
    	UE_LOG(LogTemp, Warning, TEXT("  bGameModeWaveActive: %s"), bGameModeWaveActive ? TEXT("YES") : TEXT("NO"));
    	UE_LOG(LogTemp, Warning, TEXT("  bAnySpawnerActive: %s"), bAnySpawnerActive ? TEXT("YES") : TEXT("NO"));
    	UE_LOG(LogTemp, Warning, TEXT("  CurrentMonsterCount: %d"), CurrentMonsterCount)
    	
        // 조건 1: GameMode나 스포너에서 웨이브 시작
        // 조건 2: 필드에 몬스터가 있으면 즉시 시작 (새로 추가!)
        if (bGameModeWaveActive || bAnySpawnerActive || CurrentMonsterCount > 0)
        {
            bWaveActive = true;
            bWaveCompleted = false;
            LastWaveMonsterCount = 0;
            MonsterKillCount = 0;

            // 웨이브 시작 전 기존 몬스터 목록을 저장
            PreExistingMonsters.Empty();
            for (ATNonPlayerCharacter* Monster : TrackedMonsters)
            {
                PreExistingMonsters.Add(Monster);
            }

            WaveSpawnedMonsters.Empty();
            TotalWaveMonsters = 0;
            
            UE_LOG(LogTemp, Warning, TEXT("🔥 WAVE STARTED! Reason: Field monsters detected (%d)"), CurrentMonsterCount);

            if (PlayerUIWidget)
            {
                PlayerUIWidget->ShoWEnemyIncomingAlarm();
                
                FTimerHandle AlarmTimer;
                GetWorld()->GetTimerManager().SetTimer(
                    AlarmTimer,
                    [this]()
                    {
                        if (PlayerUIWidget)
                        {
                            PlayerUIWidget->HideEnemyIncomingAlarm();
                        }
                    },
                    3.0f,
                    false);
            }
        }
    }
    
    // ✅ 핵심 수정: 필드 몬스터와 웨이브 몬스터 모두 카운트!
    if (bWaveActive && !bWaveCompleted)
    {
        // 새로 스폰된 몬스터 감지 (기존 로직)
        for (ATNonPlayerCharacter* Monster : TrackedMonsters)
        {
            if (!WaveSpawnedMonsters.Contains(Monster) && 
                !PreExistingMonsters.Contains(Monster))
            {
                WaveSpawnedMonsters.Add(Monster);
                TotalWaveMonsters++;
                UE_LOG(LogTemp, Warning, TEXT("📍 NEW wave monster detected: %s"), *Monster->GetName());
            }
        }

        WaveSpawnedMonsters.RemoveAll([](ATNonPlayerCharacter* Monster) {
            return !Monster || Monster->GetCurrentHP() <= 0;
        });

        // ✅ 수정된 킬 카운트 로직: 전체 몬스터 수 기준으로 계산
        if (CurrentMonsterCount < LastFrameMonsterCount)
        {
            int32 KilledCount = LastFrameMonsterCount - CurrentMonsterCount;

            for (int32 i = 0; i < KilledCount; i++)
            {
                MonsterKillCount++;
                UE_LOG(LogTemp, Warning, TEXT("🗡️ Monster killed! Count: %d/%d"), MonsterKillCount, SpawnerBasedMax);
            }

            // UI 업데이트
            if (PlayerUIWidget)
            {
                PlayerUIWidget->UpdateKillCount(MonsterKillCount, SpawnerBasedMax);
                UE_LOG(LogTemp, Warning, TEXT("🔄 UI Updated: %d/%d"), MonsterKillCount, SpawnerBasedMax);
            }
            
            UpdateMissionState();
        }

        // ✅ 웨이브 완료 조건 수정: 모든 몬스터 처치 시
        if (CurrentMonsterCount == 0)
        {
            bWaveCompleted = true;
            bWaveActive = false;
            WaveSpawnedMonsters.Empty();
            LastWaveMonsterCount = 0;
            
            UE_LOG(LogTemp, Warning, TEXT("🏆 WAVE COMPLETED! All monsters eliminated!"));
            UpdateMissionState();
        }
    }
	
    // 프레임 카운트 업데이트
    LastFrameMonsterCount = CurrentMonsterCount;
}

void UTUIManager::FindAllMonstersInWorld()
{
	if (UWorld* World = GetWorld())
	{
		TrackedMonsters.Empty();

		// 🔍 강화된 몬스터 검색
		int32 TotalFound = 0;
		int32 AliveCount = 0;
        
		// 월드에서 살아있는 모든 NPC 찾기
		for (TActorIterator<ATNonPlayerCharacter> ActorItr(World); ActorItr; ++ActorItr)
		{
			ATNonPlayerCharacter* Monster = *ActorItr;
			TotalFound++;
            
			if (Monster)
			{
				float HP = Monster->GetCurrentHP();
				// UE_LOG(LogTemp, Warning, TEXT("🔍 Found Monster: %s (HP: %.1f)"), 
				// 	   *Monster->GetName(), HP);
                
				if (HP > 0)
				{
					TrackedMonsters.Add(Monster);
					AliveCount++;
				}
			}
		}
        
		// UE_LOG(LogTemp, Warning, TEXT("🔍 Monster Search Results: %d total found, %d alive"), 
		// 	   TotalFound, AliveCount);
	}
}


void UTUIManager::UpdateWaveInfoFromSpawners()
{
	// 모든 스포너의 최대 스폰 수 합계 계산
	int32 TotalMaxMonsters = 0;
	for (ATEnemySpawner* Spawner : RegisteredSpawners)
	{
		if (Spawner)
		{
			TotalMaxMonsters += Spawner->MaxSpawnCount;
		}
	}
	
	if (TotalMaxMonsters > 0)
	{
		RemainingMonsters = TotalMaxMonsters;
		MonsterKillCount = 0;
		bWaveCompleted = false;

		// 초기 UI 업데이트
		if (PlayerUIWidget)
		{
			PlayerUIWidget->UpdateKillCount(0,TotalMaxMonsters);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Wave info updated: %d total monsters expected"), TotalMaxMonsters);
	}
}

void UTUIManager::MoveToNextCapturePoint()
{
	if (AllCapturePoints.IsValidIndex(CurrentCaptureIndex+1))
	{
		CurrentCaptureIndex++;
		CurrentCapturePoint=AllCapturePoints[CurrentCaptureIndex];
		CapturePointName=CurrentCapturePoint->GetName();

		UE_LOG(LogTemp, Warning, TEXT("Moved to capture point %d: %s"), 
			   CurrentCaptureIndex + 1, *CapturePointName);
		
	}
}

void UTUIManager::UnlockWeapon()
{
	bWeaponUnlocked=true;
	MoveToNextCapturePoint(); //2번쨰 거점으로 이동

	UE_LOG(LogTemp, Warning, TEXT("Moved to capture point %d: %s"), 
			   CurrentCaptureIndex + 1, *CapturePointName);
}



void UTUIManager::UpdateAllUI()
{
	// 기존 UI 업데이트
	if (ATPlayerCharacter* PC=Cast<ATPlayerCharacter>(PlayerCharacter))
	{
		CurrentWeapon=PC->CurrentWeapon;
	}
	
	UpdatePlayerHP();
	UpdatePlayerAmmo();
	UpdateWeaponInfo();
	
// 거점 상태 감시 및 UI 업데이트
    if (CurrentCapturePoint && PlayerUIWidget)
    {
        if (CurrentCapturePoint->bPlayerInArea)
        {
            bNearCapturePoint = true;
            
            if (!PlayerUIWidget->IsCaptureUIVisible())
            {
                UE_LOG(LogTemp, Warning, TEXT("Showing capture UI"));
                ShowCaptureUI(CapturePointName);
            }
            
            float ProgressPercent = CurrentCapturePoint->CapturePercent / 100.0f;
            UpdateCaptureProgress(ProgressPercent);

            // ⭐ 거점 완료시 GameMode에 알림 추가
            if (CurrentCapturePoint->CapturePercent >= 100.0f)
            {
                if (CurrentCaptureIndex == 0 && !bFirstCaptureCompleted)
                {
                    bFirstCaptureCompleted = true;
                    bCapturePhase = false;
                    HideCaptureUI();
                    
                    // ⭐ GameMode에 거점 완료 알림
                    if (GameModeRef)
                    {
                        GameModeRef->LastCapturedPoint = CurrentCapturePoint;
                        //GameModeRef->OnCapturePointCompleted(); // 보스 한방에 나와서 주석처리함..(기인)
                        UE_LOG(LogTemp, Warning, TEXT("Notified GameMode: First capture completed"));
                    }
                    
                    UE_LOG(LogTemp, Warning, TEXT("First capture point completed!"));
                    UpdateMissionState();
                }
                else if (CurrentCaptureIndex == 1 && !bSecondCaptureCompleted)
                {
                    bSecondCaptureCompleted = true;
                    bCapturePhase = false;
                    HideCaptureUI();
                    
                    // ⭐ GameMode에 거점 완료 알림
                    if (GameModeRef)
                    {
                        GameModeRef->LastCapturedPoint = CurrentCapturePoint;
                        //GameModeRef->OnCapturePointCompleted();// 보스 한방에 나와서 주석처리함..(기인)
                        UE_LOG(LogTemp, Warning, TEXT("Notified GameMode: Second capture completed"));
                    }
                    
                    UE_LOG(LogTemp, Warning, TEXT("Second capture point completed!"));
                    UpdateMissionState();
                }
            }
        }
        else
        {
            bNearCapturePoint = false;
            if (PlayerUIWidget->IsCaptureUIVisible())
            {
                UE_LOG(LogTemp, Warning, TEXT("Hiding capture UI"));
                HideCaptureUI();
            }
        }
    }
    else
    {
        if (!CurrentCapturePoint)
        {
            UE_LOG(LogTemp, Error, TEXT("CurrentCapturePoint is NULL!"));
        }
        if (!PlayerUIWidget)
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerUIWidget is NULL!"));
        }
    }
}

void UTUIManager::RespawnGameUI()
{
	UE_LOG(LogTemp, Warning, TEXT("🔄 Player Respawning - Updating UI..."));

	//리스폰 시에는 게임 진행상황은 유지, 플레이어 관련만 초기화

	if (PlayerUIWidget)
	{
		//플레이어 HP 초기화 (최대 HP로 복구)
		if (PlayerCharacter)
		{
			float MaxHP=PlayerCharacter->GetMaxHP();
			PlayerUIWidget->UpdateHPBar(MaxHP,MaxHP);
		}
		
		// 무기 정보 갱신 (현재 무기 상태 반영)
		if (ATPlayerCharacter* PC=Cast<ATPlayerCharacter>(PlayerCharacter))
		{
			CurrentWeapon=PC->CurrentWeapon;
			if (CurrentWeapon)
			{
				//탄약 정보 업데이트
				PlayerUIWidget->UpdateAmmoInfo(CurrentWeapon->GetCurrentAmmo(),CurrentWeapon->GetTotalAmmo());

				//무기 이름 업데이트
				FString WeaponName=CurrentWeapon->GetWeaponTypeString();
				PlayerUIWidget->UpdateWeaponName(WeaponName);

				//test log
				UE_LOG(LogTemp,Warning,TEXT("Weapon info updated: %s"),*WeaponName);
				
			}
		}

		// 점령 UI 상태 갱신 (플레이어가 죽기전에 거점 근처에 있엇다면)
		if (CurrentCapturePoint && CurrentCapturePoint->bPlayerInArea)
		{
			ShowCaptureUI(CapturePointName);
			float ProgressPercent=CurrentCapturePoint->CapturePercent/100.0f;
			UpdateCaptureProgress(ProgressPercent);
		}
		else
		{
			HideCaptureUI();
		}

		//알람 UI 갱신
		PlayerUIWidget->HideEnemyIncomingAlarm();
	}

	// 현재 미션 상태 재확인 (게임 진행상황에 맞게)
	UpdateMissionState();

	UE_LOG(LogTemp, Warning, TEXT("✅ Player Respawn UI Update Complete!"));
	
}


void UTUIManager::RestartGameUI()
{
	UE_LOG(LogTemp,Error,TEXT("Resetting Game UI..."));

	//미션 관련 변수 초기화
	bWaveActive=false;
	bWaveCompleted=false;
	bFirstCaptureCompleted=false;
	bSecondCaptureCompleted=false;
	bWeaponUnlocked=false;
	bNearCapturePoint=false;
	bCapturePhase=false;

	//몬스터 카운트 초기화
	MonsterKillCount=0;
	RemainingMonsters=0;
	LastFrameMonsterCount=0;
	LastWaveMonsterCount=0;
	TotalWaveMonsters=0;

	//히트 감지 변수 초기화
	LastWeaponAmmo=-1;
	LastMonsterHPs.Empty();

	//무기 관련 초기화
	bWeaponSpawned=false;
	bWeaponPickedUp=false;
	bSecondWeaponPickedUp=false;

	//거점 관련 초기화
	CurrentCaptureIndex=0;
	if (AllCapturePoints.IsValidIndex(0))
	{
		CurrentCapturePoint=AllCapturePoints[0];
		CapturePointName=CurrentCapturePoint->GetName();
	}

	//배열 초기화
	TrackedMonsters.Empty();
	WaveSpawnedMonsters.Empty();
	PreExistingMonsters.Empty();

	//UI요소 초기화
	if (PlayerUIWidget)
	{
		//초기 미션 설정
		CurrentMissionObjective = TEXT("Mission:");
		PlayerUIWidget->UpdateMissionObjective(TEXT("Mission:"));

		// 킬 카운트 초기화
		PlayerUIWidget->UpdateKillCount(0,0);

		//점령 UI 숨기기
		PlayerUIWidget->HideCaptureUI();

		// 알람 숨기기
		PlayerUIWidget->HideEnemyIncomingAlarm();

		// 🔧 실제 플레이어 정보로 업데이트
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (APawn* PlayerPawn = PC->GetPawn()) //지역변수명 다르게
			{
				if (ATPlayerCharacter* Player = Cast<ATPlayerCharacter>(PlayerPawn))
				{
					// 실제 플레이어 체력으로 업데이트
					float CurrentHealth = Player->GetCurrentHP();
					float MaxHealth = Player->GetMaxHP();
					PlayerUIWidget->UpdateHPBar(CurrentHealth, MaxHealth);
            
					UE_LOG(LogTemp, Warning, TEXT("🔧 Health updated from player: %.1f/%.1f"), 
						   CurrentHealth, MaxHealth);

					// 🔧 플레이어가 가진 무기 정보 업데이트
					ATWeaponBase* PlayerWeapon = Player->GetCurrentWeapon();
					if (PlayerWeapon)
					{
						// 무기 탄약 정보 업데이트
						int32 CurrentAmmo = PlayerWeapon->GetCurrentAmmo();
						int32 TotalAmmo = PlayerWeapon->GetTotalAmmo();
						PlayerUIWidget->UpdateAmmoInfo(CurrentAmmo, TotalAmmo);
                
						// 무기 이름 업데이트
						FString WeaponName = PlayerWeapon->GetWeaponTypeString();
						PlayerUIWidget->UpdateWeaponName(WeaponName);
                
						UE_LOG(LogTemp, Warning, TEXT("🔧 Weapon updated: %s (%d/%d)"), 
							   *WeaponName, CurrentAmmo, TotalAmmo);
					}
					else
					{
						// 무기가 없다면 기본값 (권총 7/100)
						PlayerUIWidget->UpdateAmmoInfo(7, 100);
						PlayerUIWidget->UpdateWeaponName(TEXT("Pistol"));
						UE_LOG(LogTemp, Warning, TEXT("🔧 No weapon found, using default pistol (7/100)"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("❌ Failed to cast to ATPlayerCharacter"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("❌ PlayerPawn is NULL"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Failed to get PlayerController for UI update"));
		}

		
	}

	
	//타이머 초기화
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(MonsterMonitorTimer);
		GetWorld()->GetTimerManager().ClearTimer(UIUpdateTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(HitDetectionTimer);

		//잠시 지연후 재시작
		FTimerHandle DelayTimer;
		GetWorld()->GetTimerManager().SetTimer(
			DelayTimer,
			[this]()
			{
				//모니터링 재시작
		          StartMonitoringMonsters();
		           StartHitDetection();

		           //UI업데이트 타이머 재시작
		          GetWorld()->GetTimerManager().SetTimer(
			      UIUpdateTimerHandle,
			      this,
			       &UTUIManager::UpdateAllUI,
			0.1f,
			true
			);
				UE_LOG(LogTemp, Warning, TEXT("✅ Monitoring systems restarted after delay"));
			},
			0.5f,
			false
			);

		
	}

	//스포너 재등록 및 웨이브 정보 업데이트
	FindAndRegisterEnemySpawners();
	UpdateWaveInfoFromSpawners();

	//1.5초후 첫 미션 업데이트
	FTimerHandle UpdateMissionTimer;
	GetWorld()->GetTimerManager().SetTimer(
		RestartMissionTimer,
		[this]()
		{
			UpdateMissionState();
		},
		1.5f,
		false
		);

	UE_LOG(LogTemp,Warning,TEXT("Game UI reset completed!"));
	
}

void UTUIManager::StartHitDetection()
{
    if (GetWorld() && !HitDetectionTimer.IsValid())
    {
        GetWorld()->GetTimerManager().SetTimer(
            HitDetectionTimer,
            this,
            &UTUIManager::CheckForHits,
            0.1f,  // 0.1초마다 체크
            true   // 반복
        );
        
        UE_LOG(LogTemp, Warning, TEXT("🎯 Hit detection started"));
    }
}

void UTUIManager::StopHitDetection()
{
    if (GetWorld() && HitDetectionTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(HitDetectionTimer);
        UE_LOG(LogTemp, Warning, TEXT("🎯 Hit detection stopped"));
    }
}

void UTUIManager::CheckForHits()
{
    // 플레이어와 무기 확인
    if (!PlayerCharacter) return;
    
    ATPlayerCharacter* Player = Cast<ATPlayerCharacter>(PlayerCharacter);
    if (!Player) return;
    
    ATWeaponBase* Weapon = Player->CurrentWeapon;
    if (!Weapon) return;
    
    // 🔫 무기 발사 감지 (탄약 변화)
    int32 CurrentAmmo = Weapon->GetCurrentAmmo();
    
    if (LastWeaponAmmo > 0 && CurrentAmmo < LastWeaponAmmo)
    {
        UE_LOG(LogTemp, Warning, TEXT("🔫 Weapon fired! Ammo: %d -> %d"), LastWeaponAmmo, CurrentAmmo);
        
        // 🎯 몬스터 HP 변화 체크
        if (CheckMonsterHPChanges())
        {
            // 히트마커 표시!
            if (PlayerUIWidget)
            {
                PlayerUIWidget->ShowHitMarker();
                UE_LOG(LogTemp, Warning, TEXT("🎯 HIT DETECTED! Showing red hitmarker"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("❌ Miss detected - no monster HP change"));
        }
    }
    
    // 상태 업데이트
    LastWeaponAmmo = CurrentAmmo;
    UpdateMonsterHPList();
}

bool UTUIManager::CheckMonsterHPChanges()
{
    // 현재 추적 중인 모든 몬스터의 HP 체크
    for (int32 i = 0; i < TrackedMonsters.Num(); i++)
    {
        if (TrackedMonsters[i] && IsValid(TrackedMonsters[i]))
        {
            float CurrentHP = TrackedMonsters[i]->GetCurrentHP();
            
            // 이전 HP와 비교
            if (i < LastMonsterHPs.Num())
            {
                if (CurrentHP < LastMonsterHPs[i])
                {
                    UE_LOG(LogTemp, Warning, TEXT("🩸 Monster %s HP decreased: %.1f -> %.1f"), 
                           *TrackedMonsters[i]->GetName(), LastMonsterHPs[i], CurrentHP);
                    return true; // 히트 감지!
                }
            }
        }
    }
    
    return false; // 히트 없음
}

void UTUIManager::UpdateMonsterHPList()
{
    LastMonsterHPs.Empty();
    
    // 현재 추적 중인 모든 몬스터의 HP 저장
    for (ATNonPlayerCharacter* Monster : TrackedMonsters)
    {
        if (Monster && IsValid(Monster))
        {
            LastMonsterHPs.Add(Monster->GetCurrentHP());
        }
    }
}

// ===== 기존 TestHitMarker 함수 수정 =====
void UTUIManager::TestHitMarker()
{
    if (PlayerUIWidget)
    {
        PlayerUIWidget->ShowHitMarker();
        UE_LOG(LogTemp, Warning, TEXT("🎯 Test red hitmarker called"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerUIWidget is null!"));
    }
}