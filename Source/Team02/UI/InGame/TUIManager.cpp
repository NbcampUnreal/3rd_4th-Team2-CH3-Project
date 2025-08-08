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
#include "Spawner/TEnemySpawner.h"
#include "TAIBossMonster/TAIBossMonster.h"
#include "TGameMode.h"
#include "UnifiedBuffer.h"

void UTUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	//테스트 로그
	UE_LOG(LogTemp,Warning,TEXT("UIManager Initialized!!"));
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
			
			// 거점 자동 검색 및 등록(UI 생성 이후)
			FindAndRegisterCapturePoints();

			// 스포너 및 몬스터 모너터링 시작
			FindAndRegisterEnemySpawners();
			StartMonitoringMonsters();

			// 첫 임무 시작시 0.5초 지연
			FTimerHandle InitMissionTimer;
			GetWorld()->GetTimerManager().SetTimer(
				InitMissionTimer,
				[this]()
				{
					UpdateMissionState();
				},
				0.5f,
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
	if (PlayerUIWidget && PlayerCharacter)
	{
		PlayerUIWidget->UpdateHPBar(PlayerCharacter->GetCurrentHP(),PlayerCharacter->GetMaxHP());
	}
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

void UTUIManager::UpdateMissionState()
{
    FString NewObjective;
    
    // ⭐ 현재 상태를 더 자세히 로깅
    UE_LOG(LogTemp, Warning, TEXT("🎯 Mission State Update:"));
    UE_LOG(LogTemp, Warning, TEXT("  bBossPhase: %s"), bBossPhase ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("  bSecondCaptureCompleted: %s"), bSecondCaptureCompleted ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("  bFirstCaptureCompleted: %s"), bFirstCaptureCompleted ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("  bWeaponUnlocked: %s"), bWeaponUnlocked ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("  bWaveCompleted: %s"), bWaveCompleted ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("  bWaveActive: %s"), bWaveActive ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("  CurrentCaptureIndex: %d"), CurrentCaptureIndex);
    UE_LOG(LogTemp, Warning, TEXT("  MonsterKillCount: %d"), MonsterKillCount);
    UE_LOG(LogTemp, Warning, TEXT("  TrackedMonsters.Num(): %d"), TrackedMonsters.Num());

    if (bBossPhase)
    {
        NewObjective = TEXT("Defeat the boss!");
    }
    else if (bSecondCaptureCompleted)
    {
        NewObjective = TEXT("Prepare for final boss battle!");

        if (!bBossPhase)
        {
            FTimerHandle BossTimer;
            GetWorld()->GetTimerManager().SetTimer(
                BossTimer,
                [this]()
                {
                    bBossPhase = true;
                    UpdateMissionState();
                },
                5.0f,
                false
            );
        }
    }
    else if (bWeaponUnlocked && CurrentCaptureIndex == 1)
    {
        if (bNearCapturePoint && bCapturePhase)
        {
            NewObjective = TEXT("Capture the second control point!");
        }
        else if (bNearCapturePoint && !bCapturePhase)
        {
            NewObjective = TEXT("Enter the control point to begin capture!");
            bCapturePhase = true;
        }
        else
        {
            NewObjective = TEXT("Move to second control point!");
        }
    }
    else if (bFirstCaptureCompleted && !bWeaponUnlocked)
    {
        NewObjective = TEXT("First area captured! Unlocking weapon...");

        FTimerHandle WeaponTimer;
        GetWorld()->GetTimerManager().SetTimer(
            WeaponTimer,
            [this]()
            {
                UnlockWeapon();
                bCapturePhase = false;
            },
            2.0f,
            false
        );
    }
    else if (bWaveCompleted && CurrentCaptureIndex == 0)
    {
        if (bNearCapturePoint && bCapturePhase)
        {
            NewObjective = TEXT("Capture the first control point!");
        }
        else if (bNearCapturePoint && !bCapturePhase)
        {
            NewObjective = TEXT("Enter the control point to begin capture!");
            bCapturePhase = true;
        }
        else
        {
            NewObjective = TEXT("Move to first control point!");
        }
    }
    else if (bWaveActive || TrackedMonsters.Num() > 0)
    {
        // ⭐ 스포너 기반 남은 수 계산 개선
        int32 SpawnerBasedMax = 0;
        for (ATEnemySpawner* Spawner : RegisteredSpawners)
        {
            if (Spawner) 
            {
                SpawnerBasedMax += Spawner->MaxSpawnCount;
            }
        }
        
        int32 RemainingCount = FMath::Max(0, SpawnerBasedMax - MonsterKillCount);
        NewObjective = FString::Printf(TEXT("Eliminate enemies (%d remaining)"), RemainingCount);
        
        UE_LOG(LogTemp, Warning, TEXT("📊 Enemy Objective: %d killed, %d remaining"), 
               MonsterKillCount, RemainingCount);
    }
    else
    {
        NewObjective = TEXT("Eliminate all enemies");
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

		UE_LOG(LogTemp,Warning,TEXT("Monster monitering startered!"));
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
    FindAllBossesInWorld();

    int32 CurrentMonsterCount = TrackedMonsters.Num();
    int32 CurrentBossCount = TrackedBosses.Num();

    // ⭐ GameMode의 웨이브 상태 확인
    bool bGameModeWaveActive = false;
    if (GameModeRef)
    {
        bGameModeWaveActive = GameModeRef->bIsWaveActive;
    }

    // ⭐ 스포너 상태 체크
    bool bAnySpawnerActive = false;
    int32 TotalSpawnersCompleted = 0;
    int32 SpawnerBasedMax = 0;
    
    for (ATEnemySpawner* Spawner : RegisteredSpawners)
    {
        if (Spawner)
        {
            SpawnerBasedMax += Spawner->MaxSpawnCount;
            int32 CurrentSpawned = Spawner->GetCurrentSpawned();
            int32 MaxSpawn = Spawner->MaxSpawnCount;
            
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
    
    // 웨이브 시작 감지
    if ((bGameModeWaveActive || bAnySpawnerActive) && !bWaveActive && !bWaveCompleted)
    {
        bWaveActive = true;
        bWaveCompleted = false;

    	LastWaveMonsterCount=0;
    	MonsterKillCount=0;

        // 웨이브 시작 전 기존 몬스터 목록을 저장
        PreExistingMonsters.Empty();
        for (ATNonPlayerCharacter* Monster : TrackedMonsters)
        {
            PreExistingMonsters.Add(Monster);
        }

        // 웨이브 몬스터 목록 초기화
        WaveSpawnedMonsters.Empty();
        TotalWaveMonsters = 0;
        
        UE_LOG(LogTemp, Warning, TEXT("🔥 WAVE STARTED! Wave monsters will be tracked separately."));

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
    
    // ⭐ 웨이브 중일 때 새로 스폰된 몬스터 감지
    if (bWaveActive && !bWaveCompleted)
    {
        // 현재 월드의 모든 몬스터와 이전에 기록된 웨이브 몬스터 비교
        for (ATNonPlayerCharacter* Monster : TrackedMonsters)
        {
            // 기존 몬스터가 아니고, 웨이브 목록에도 없는 경우만 추가
            if (!WaveSpawnedMonsters.Contains(Monster) && 
                !PreExistingMonsters.Contains(Monster))
            {
                WaveSpawnedMonsters.Add(Monster);
                TotalWaveMonsters++;
                UE_LOG(LogTemp, Warning, TEXT("📍 NEW wave monster detected: %s (Total: %d)"), 
                       *Monster->GetName(), TotalWaveMonsters);
            }
        }

        // 죽은 웨이브 몬스터 제거
        WaveSpawnedMonsters.RemoveAll([](ATNonPlayerCharacter* Monster) {
            return !Monster || Monster->GetCurrentHP() <= 0;
        });
    }
	

    // 웨이브 몬스터 기준으로 처치 카운트 (개선된 로직)
    int32 CurrentWaveMonsterCount = WaveSpawnedMonsters.Num();

    //  초기화 시점에서 LastWaveMonsterCount를 올바르게 설정
    if (bWaveActive && LastWaveMonsterCount == 0 && CurrentWaveMonsterCount > 0)
    {
        LastWaveMonsterCount = CurrentWaveMonsterCount;
        UE_LOG(LogTemp, Warning, TEXT("🎯 Initial wave monster count set: %d"), CurrentWaveMonsterCount);
    }

    // 몬스터가 죽었을 때 처치 카운트 증가
    if (bWaveActive && !bWaveCompleted && CurrentWaveMonsterCount< LastWaveMonsterCount)
    {
        int32 KilledCount = LastWaveMonsterCount - CurrentWaveMonsterCount;

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
        
        // 즉시 미션 상태 업데이트
        UpdateMissionState();
    }

    // 웨이브 완료 조건 개선
    if (bWaveActive && !bWaveCompleted)
    {
        bool bAllWaveMonstersEliminated = (CurrentWaveMonsterCount == 0);
        bool bAllSpawnersCompleted = (TotalSpawnersCompleted == RegisteredSpawners.Num());
        bool bKillCountMet = (MonsterKillCount >= SpawnerBasedMax);

        // 웨이브 완료 조건 계산
        bool bWaveComplete = (MonsterKillCount>=SpawnerBasedMax);;

        UE_LOG(LogTemp, Warning, TEXT("🔍 Wave Completion Check:"));
        UE_LOG(LogTemp, Warning, TEXT("  Wave Monsters: %d"), CurrentWaveMonsterCount);
        UE_LOG(LogTemp, Warning, TEXT("  Kill Count: %d/%d"), MonsterKillCount, SpawnerBasedMax);
        UE_LOG(LogTemp, Warning, TEXT("  Spawners Completed: %d/%d"), TotalSpawnersCompleted, RegisteredSpawners.Num());
        UE_LOG(LogTemp, Warning, TEXT("  All Eliminated: %s"), bAllWaveMonstersEliminated ? TEXT("YES") : TEXT("NO"));
        UE_LOG(LogTemp, Warning, TEXT("  Kill Count Met: %s"), bKillCountMet ? TEXT("YES") : TEXT("NO"));
        UE_LOG(LogTemp, Warning, TEXT("  Wave Complete: %s"), bWaveComplete ? TEXT("YES") : TEXT("NO"));
        
        if (bWaveComplete)
        {
            bWaveCompleted = true;
            bWaveActive = false;
            WaveSpawnedMonsters.Empty();
        	LastWaveMonsterCount=0;
            
            UE_LOG(LogTemp, Warning, TEXT("🏆 WAVE COMPLETED! All wave monsters eliminated!"));
            
            // ⭐ 웨이브 완료 후 즉시 미션 업데이트
            UpdateMissionState();
        }
    }

    // 보스 처치 감지
    if (bBossPhase && CurrentBossCount < LastFrameBossCount)
    {
        UE_LOG(LogTemp, Warning, TEXT("🏆 Boss DEFEATED!! Game Complete!"));
        bBossPhase = false;
        SetMissionObjective(TEXT("Victory! Boss Defeated!"));
        OnVictoryEvent.Broadcast();
    }

    // ⭐ 프레임 카운트 업데이트
    LastFrameMonsterCount = CurrentMonsterCount;
    LastFrameBossCount = CurrentBossCount;
    LastWaveMonsterCount = CurrentWaveMonsterCount;
}

void UTUIManager::FindAllMonstersInWorld()
{
	if (UWorld* World=GetWorld())
	{
		TrackedMonsters.Empty();

		//  월드에서 살아있는 모든 NPC 찾기
		for (TActorIterator<ATNonPlayerCharacter> ActorItr(World); ActorItr; ++ActorItr)
		{
			ATNonPlayerCharacter* Monster= *ActorItr;
			if (Monster && Monster->GetCurrentHP()>0)
			{
				TrackedMonsters.Add(Monster);
			}
		}
	}
}

void UTUIManager::FindAllBossesInWorld()
{
	if (UWorld* World = GetWorld())
	{
		TrackedBosses.Empty();
        
		for (TActorIterator<ATAIBossMonster> ActorItr(World); ActorItr; ++ActorItr)
		{
			ATAIBossMonster* Character = *ActorItr;
			if (Character)
			{
				//보스 블루프린트인지 확인(이름으로)
				FString ActorName= Character->GetClass()->GetName();
				if (ActorName.Contains(TEXT("BP_TAIBossMonster")) ||
					ActorName.Contains(TEXT("BossMonster")))
				{
					UE_LOG(LogTemp,Warning,TEXT("Found boss by name: %s, HP: %.1f"),
						*ActorName,Character->GetCurrentHP());

					if (Character->GetCurrentHP()>0)
					{
						//ATAIBossMonster* 로 캐스팅해서 저장
						if (ATAIBossMonster* Boss=Cast<ATAIBossMonster>(Character))
						{
							TrackedBosses.Add(Boss);
						}
					}
				}
			}
		}
        
		UE_LOG(LogTemp, Warning, TEXT("Total alive bosses: %d"), TrackedBosses.Num());
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
                        GameModeRef->OnCapturePointCompleted();
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
                        GameModeRef->OnCapturePointCompleted();
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

void UTUIManager::Deinitialize()
{
	//타이머 세팅
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(UIUpdateTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(MonsterMonitorTimer);
	}
	Super::Deinitialize();
}
