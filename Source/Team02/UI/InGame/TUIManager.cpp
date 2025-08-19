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
#include "TGameMode.h"


void UTUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
}

void UTUIManager::Deinitialize()
{
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
	if (!PlayerUIWidgetClass)
	{
		PlayerUIWidgetClass = LoadClass<UTPlayerUIWidget>(nullptr, TEXT("/Game/Team02/UI/InGame/WBP_PlayerUI.WBP_PlayerUI_C"));
        
		if (!PlayerUIWidgetClass)
		{
			return;
		}
		
	}

	if (GetWorld())
	{
		GameModeRef = Cast<ATGameMode>(GetWorld()->GetAuthGameMode());
		
		PlayerUIWidget = CreateWidget<UTPlayerUIWidget>(GetWorld(), PlayerUIWidgetClass);
		if (PlayerUIWidget)
		{
			PlayerUIWidget->AddToViewport();
			
			// UI 업데이트 타이머
			GetWorld()->GetTimerManager().SetTimer(
				UIUpdateTimerHandle,
				this,
				&UTUIManager::UpdateAllUI,
				0.1f,
				true);

			
			StartHitDetection(); //히트 감지 시작
			FindAndRegisterCapturePoints(); // 거점 자동 검색 및 등록(UI 생성 이후)
			FindAndRegisterEnemySpawners(); // 스포너 및 몬스터 모너터링 시작
			StartMonitoringMonsters();
			
			CurrentMissionObjective=TEXT("Mission:"); 
			PlayerUIWidget->UpdateMissionObjective(TEXT("Mission:"));
			
			FTimerHandle InitMissionTimer; // 첫 임무 시작시 1.5초 지연
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
	}
}


void UTUIManager::SetPlayerCharacter(ATCharacterBase* PlayerChar)
{
	PlayerCharacter=PlayerChar;
	
	if (ATPlayerCharacter*PC=Cast<ATPlayerCharacter>(PlayerChar))
	{
		CurrentWeapon=PC->CurrentWeapon;
	}
}

void UTUIManager::UpdatePlayerHP()
{
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
}

void UTUIManager::UpdatePlayerAmmo()
{
	if (PlayerUIWidget && CurrentWeapon)
	{
		PlayerUIWidget->UpdateAmmoInfo(
			CurrentWeapon->GetCurrentAmmo(),CurrentWeapon->GetTotalAmmo());
	}
}

void UTUIManager::UpdateWeaponInfo()
{
	if (CurrentWeapon != PreviousWeapon) // 무기 변경 감지
	{
		if (CurrentWeapon && PlayerUIWidget)
		{
			FString WeaponName=CurrentWeapon->GetWeaponTypeString();
			PlayerUIWidget->UpdateWeaponName(WeaponName);
			
			if (bFirstCaptureCompleted && !bWeaponPickedUp)
			{
				if (WeaponName != TEXT("Pistol") && WeaponName !=TEXT("No Weapon"))
				{
					bWeaponPickedUp=true;
					bWeaponUnlocked=true;
					MoveToNextCapturePoint(); // 2번쨰 거점으로 이동
					UpdateMissionState(); // 미션 업데이트
				}
			}
			//두번쨰 무기 습득 감지(2거점 가기 전 라이플)
			else if (bWeaponPickedUp && !bSecondWeaponPickedUp && CurrentCaptureIndex==1)
			{
				if (WeaponName != TEXT("Pistol") && WeaponName !=TEXT("No Weapon"))
				{
					if (PreviousWeapon && WeaponName != PreviousWeapon->GetWeaponTypeString())
					{
						bSecondWeaponPickedUp=true;
						UpdateMissionState();
					}
				}
			}
		}
		
		else if (!CurrentWeapon && PlayerUIWidget)
		{
			PlayerUIWidget->UpdateWeaponName(TEXT("No Weapon"));
		}
		PreviousWeapon=CurrentWeapon; // 이전 무기 업데이트
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
    	
        for (TActorIterator<ATCapturePoint> ActorItr(World); ActorItr; ++ActorItr) // 모든 거점 찾기
        {
            ATCapturePoint* CapturePoint = *ActorItr;
            if (CapturePoint)
            {
                FString ActorName = CapturePoint->GetName();

            	// 이름으로 1거점/2거점 구분
                if (ActorName.Contains(TEXT("First")) || 
                    ActorName.Contains(TEXT("1")) || 
                    ActorName.Contains(TEXT("One")))
                {
                    FirstPoint = CapturePoint;
                }
                else if (ActorName.Contains(TEXT("Second")) || 
                         ActorName.Contains(TEXT("2")) || 
                         ActorName.Contains(TEXT("Two")))
                {
                    SecondPoint = CapturePoint;
                }
            }
        }

        // 순서대로 배열에 추가 (1거점 먼저, 2거점 나중에)
        if (FirstPoint)
        {
            AllCapturePoints.Add(FirstPoint);
        }
        if (SecondPoint)
        {
            AllCapturePoints.Add(SecondPoint);
        }
        if (AllCapturePoints.Num() >= 2)
        {
            // 첫 번째 거점을 현재 거점으로 설정
            CurrentCapturePoint = AllCapturePoints[CurrentCaptureIndex];
            CapturePointName = CurrentCapturePoint->GetName();
        	
        }
    }
}

void UTUIManager::SetMissionObjective(const FString& NewObjective)
{
	CurrentMissionObjective=NewObjective;

	if (PlayerUIWidget)
	{
		PlayerUIWidget->UpdateMissionObjective(NewObjective);
	}
}

void UTUIManager::UpdateMissionProgress()
{
	UpdateMissionState();
}

void UTUIManager::UpdateMissionState()
{
    FString NewObjective;
	
    if (bSecondCaptureCompleted)
    {
        NewObjective = TEXT("Victory!"); 
        OnVictoryEvent.Broadcast();
    }
    else if (bSecondWeaponPickedUp && CurrentCaptureIndex == 1)
    {
        if (bNearCapturePoint && bCapturePhase)
        {
            NewObjective = TEXT("Capture Point 2");  
        }
        else if (bNearCapturePoint && !bCapturePhase)
        {
            NewObjective = TEXT("Enter Point 2");  
            bCapturePhase = true;
        }
        else
        {
            NewObjective = TEXT("Move to Point 2");
        }
    }
    else if (bWeaponPickedUp && !bSecondWeaponPickedUp && CurrentCaptureIndex == 1)
    {
        NewObjective = TEXT("Get RailGun");  
    }
    else if (bFirstCaptureCompleted && !bWeaponPickedUp)
    {
        NewObjective = TEXT("Get Shotgun");  
    }
    else if (bWaveCompleted && CurrentCaptureIndex == 0)
    {
        if (bNearCapturePoint && bCapturePhase)
        {
            NewObjective = TEXT("Capture Point 1");  
        }
        else if (bNearCapturePoint && !bCapturePhase)
        {
            NewObjective = TEXT("Enter Point 1");  
            bCapturePhase = true;
        }
        else
        {
            NewObjective = TEXT("Move to Point 1");  
        }
    }
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
        NewObjective = FString::Printf(TEXT("Kill Enemies (%d left)"), RemainingCount);  
    }
    else
    {
        NewObjective = TEXT("Kill All Enemies");  
    }
    if (CurrentMissionObjective != NewObjective)
    {
        SetMissionObjective(NewObjective);
    }
}

void UTUIManager::FindAndRegisterEnemySpawners()
{
	if (UWorld* World=GetWorld())
	{
		RegisteredSpawners.Empty();
		
		for (TActorIterator<ATEnemySpawner> ActorItr(World); ActorItr; ++ActorItr)
		{
			ATEnemySpawner* Spawner= *ActorItr;
			if (Spawner)
			{
				RegisteredSpawners.Add(Spawner);
			}
		}
		if (RegisteredSpawners.Num()>0)
		{
			UpdateWaveInfoFromSpawners();
		}
	}
}

void UTUIManager::StartMonitoringMonsters()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			MonsterMonitorTimer,
			this,
			&UTUIManager::UpdateMonsterStatus,
			0.5f,
			true);
	}
}

void UTUIManager::StopMonitoringMonsters()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(MonsterMonitorTimer);
	}
}

void UTUIManager::UpdateMonsterStatus()
{
	FindAllMonstersInWorld();
	
    int32 CurrentMonsterCount = TrackedMonsters.Num();
	
    // GameMode 웨이브 상태 확인
    bool bGameModeWaveActive = false;
    if (GameModeRef)
    {
        bGameModeWaveActive = GameModeRef->bIsWaveActive;
    }

    // 스포너 상태 상세 체크
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
        // 조건 1: GameMode나 스포너에서 웨이브 시작
        // 조건 2: 필드에 몬스터가 있으면 즉시 시작 (새로 추가)
        if (bGameModeWaveActive || bAnySpawnerActive || CurrentMonsterCount > 0)
        {
            bWaveActive = true;
            bWaveCompleted = false;
            LastWaveMonsterCount = 0;
            MonsterKillCount = 0;

            // 웨이브 시작 전 기존 몬스터 목록을 저장
            PreExistingMonsters.Empty();
            for (ATCharacterBase* Monster : TrackedMonsters)
            {
                PreExistingMonsters.Add(Monster);
            }

            WaveSpawnedMonsters.Empty();
            TotalWaveMonsters = 0;
        	

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
    
    // 필드 몬스터와 웨이브 몬스터 모두 카운트
    if (bWaveActive && !bWaveCompleted)
    {
        // 새로 스폰된 몬스터 감지 (기존 로직)
        for (ATCharacterBase* Monster : TrackedMonsters)
        {
            if (!WaveSpawnedMonsters.Contains(Monster) && 
                !PreExistingMonsters.Contains(Monster))
            {
                WaveSpawnedMonsters.Add(Monster);
                TotalWaveMonsters++;
            }
        }

        WaveSpawnedMonsters.RemoveAll([](ATCharacterBase* Monster) {
            return !Monster || Monster->GetCurrentHP() <= 0;
        });

        // 수정된 킬 카운트 로직: 전체 몬스터 수 기준으로 계산
        if (CurrentMonsterCount < LastFrameMonsterCount)
        {
            int32 KilledCount = LastFrameMonsterCount - CurrentMonsterCount;

            for (int32 i = 0; i < KilledCount; i++)
            {
                MonsterKillCount++;
            }
        	
            if (PlayerUIWidget)
            {
                PlayerUIWidget->UpdateKillCount(MonsterKillCount, SpawnerBasedMax);
            }
            
            UpdateMissionState();
        }

        //웨이브 완료 조건 수정: 모든 몬스터 처치 시
        if (CurrentMonsterCount == 0)
        {
            bWaveCompleted = true;
            bWaveActive = false;
            WaveSpawnedMonsters.Empty();
            LastWaveMonsterCount = 0;
        	
            UpdateMissionState();
        }
    }
	
    LastFrameMonsterCount = CurrentMonsterCount;
}

void UTUIManager::FindAllMonstersInWorld()
{
	if (UWorld* World = GetWorld())
	{
		TrackedMonsters.Empty();

		int32 TotalFound = 0;
		int32 AliveCount = 0;
        
		// 1. Gun 몬스터 찾기 (ATNonPlayerCharacter)
		for (TActorIterator<ATNonPlayerCharacter> ActorItr(World); ActorItr; ++ActorItr)
		{
			ATNonPlayerCharacter* Monster = *ActorItr;
			TotalFound++;
            
			if (Monster)
			{
				float HP = Monster->GetCurrentHP();
                
				if (HP > 0)
				{
					TrackedMonsters.Add(Cast<ATCharacterBase>(Monster));
					AliveCount++;
				}
			}
		}

		// 2. Sword 몬스터 찾기 (ATNonPlayerCharacterSword)
		for (TActorIterator<ATNonPlayerCharacterSword> SwordActorItr(World); SwordActorItr; ++SwordActorItr)
		{
			ATNonPlayerCharacterSword* SwordMonster = *SwordActorItr;
			TotalFound++;
            
			if (SwordMonster)
			{
				float HP = SwordMonster->GetCurrentHP();
                
				if (HP > 0)
				{
					TrackedMonsters.Add(Cast<ATCharacterBase>(SwordMonster));
					AliveCount++;
				}
			}
		}
		
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
		
	}
}

void UTUIManager::MoveToNextCapturePoint()
{
	if (AllCapturePoints.IsValidIndex(CurrentCaptureIndex+1))
	{
		CurrentCaptureIndex++;
		CurrentCapturePoint=AllCapturePoints[CurrentCaptureIndex];
		CapturePointName=CurrentCapturePoint->GetName();
		
		
	}
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
	DetectTeleportDash();
	
    if (CurrentCapturePoint && PlayerUIWidget)
    {
        if (CurrentCapturePoint->bPlayerInArea)
        {
            bNearCapturePoint = true;
            
            if (!PlayerUIWidget->IsCaptureUIVisible())
            {
                ShowCaptureUI(CapturePointName);
            }
            
            float ProgressPercent = CurrentCapturePoint->CapturePercent / 100.0f;
            UpdateCaptureProgress(ProgressPercent);
        	
            if (CurrentCapturePoint->CapturePercent >= 100.0f)
            {
                if (CurrentCaptureIndex == 0 && !bFirstCaptureCompleted)
                {
                    bFirstCaptureCompleted = true;
                    bCapturePhase = false;
                    HideCaptureUI();
                	
                    if (GameModeRef)
                    {
                        GameModeRef->LastCapturedPoint = CurrentCapturePoint;
                    }
                	
                    UpdateMissionState();
                }
                else if (CurrentCaptureIndex == 1 && !bSecondCaptureCompleted)
                {
                    bSecondCaptureCompleted = true;
                    bCapturePhase = false;
                    HideCaptureUI();
                	
                    if (GameModeRef)
                    {
                        GameModeRef->LastCapturedPoint = CurrentCapturePoint;
                    }
                	
                    UpdateMissionState();
                }
            }
        }
        else
        {
            bNearCapturePoint = false;
            if (PlayerUIWidget->IsCaptureUIVisible())
            {
                HideCaptureUI();
            }
        }
    }
}

void UTUIManager::RestartGameUI()
{
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
		CurrentMissionObjective = TEXT("Mission:");
		PlayerUIWidget->UpdateMissionObjective(TEXT("Mission:"));
		PlayerUIWidget->UpdateKillCount(0,0);
		PlayerUIWidget->HideCaptureUI();
		PlayerUIWidget->HideEnemyIncomingAlarm();
		PlayerUIWidget->ShowDashReady();

		// 실제 플레이어 정보로 업데이트
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (APawn* PlayerPawn = PC->GetPawn()) 
			{
				if (ATPlayerCharacter* Player = Cast<ATPlayerCharacter>(PlayerPawn))
				{
					float CurrentHealth = Player->GetCurrentHP();
					float MaxHealth = Player->GetMaxHP();
					PlayerUIWidget->UpdateHPBar(CurrentHealth, MaxHealth);
					
					ATWeaponBase* PlayerWeapon = Player->GetCurrentWeapon();
					if (PlayerWeapon)
					{
						int32 CurrentAmmo = PlayerWeapon->GetCurrentAmmo();
						int32 TotalAmmo = PlayerWeapon->GetTotalAmmo();
						PlayerUIWidget->UpdateAmmoInfo(CurrentAmmo, TotalAmmo);
						
						FString WeaponName = PlayerWeapon->GetWeaponTypeString();
						PlayerUIWidget->UpdateWeaponName(WeaponName);
						
					}
					else
					{
						PlayerUIWidget->UpdateAmmoInfo(7, 100);
						PlayerUIWidget->UpdateWeaponName(TEXT("Pistol"));
					}
				}
				
			}
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
				
		          StartMonitoringMonsters();
		           StartHitDetection();
				
		          GetWorld()->GetTimerManager().SetTimer(
			      UIUpdateTimerHandle,
			      this,
			       &UTUIManager::UpdateAllUI,
			0.1f,
			true
			);
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
}

void UTUIManager::StartHitDetection()
{
    if (GetWorld() && !HitDetectionTimer.IsValid())
    {
        GetWorld()->GetTimerManager().SetTimer(
            HitDetectionTimer,
            this,
            &UTUIManager::CheckForHits,
            0.1f,  
            true   
        );
    }
}

void UTUIManager::StopHitDetection()
{
    if (GetWorld() && HitDetectionTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(HitDetectionTimer);
    }
}

void UTUIManager::CheckForHits()
{
    if (!PlayerCharacter) return;
    
    ATPlayerCharacter* Player = Cast<ATPlayerCharacter>(PlayerCharacter);
    if (!Player) return;
    
    ATWeaponBase* Weapon = Player->CurrentWeapon;
    if (!Weapon) return;
	
    int32 CurrentAmmo = Weapon->GetCurrentAmmo();
    
    if (LastWeaponAmmo > 0 && CurrentAmmo < LastWeaponAmmo)
    {
        if (CheckMonsterHPChanges())
        {
            if (PlayerUIWidget)
            {
                PlayerUIWidget->ShowHitMarker();
            }
        }
    }
    
    // 상태 업데이트
    LastWeaponAmmo = CurrentAmmo;
    UpdateMonsterHPList();
}

bool UTUIManager::CheckMonsterHPChanges()
{
    for (int32 i = 0; i < TrackedMonsters.Num(); i++)
    {
        if (TrackedMonsters[i] && IsValid(TrackedMonsters[i]))
        {
            float CurrentHP = TrackedMonsters[i]->GetCurrentHP();
        	
            if (i < LastMonsterHPs.Num())
            {
                if (CurrentHP < LastMonsterHPs[i])
                {
                    return true; 
                }
            }
        }
    }
    return false; 
}

void UTUIManager::UpdateMonsterHPList()
{
    LastMonsterHPs.Empty();
	
    for (ATCharacterBase* Monster : TrackedMonsters)
    {
        if (Monster && IsValid(Monster))
        {
            LastMonsterHPs.Add(Monster->GetCurrentHP());
        }
    }
}

void UTUIManager::DetectTeleportDash()
{
    if (!PlayerCharacter || !PlayerUIWidget) return;
    
    // 팀원분의 실제 Can Dash 상태만 읽기 (위치 감지 제거)
    bool bCanDashFromBP = true; 
    if (ATPlayerCharacter* Player = Cast<ATPlayerCharacter>(PlayerCharacter))
    {
        UClass* PlayerClass = Player->GetClass();
        
        // Can Dash 변수 찾기
        for (FProperty* Property = PlayerClass->PropertyLink; Property; Property = Property->PropertyLinkNext)
        {
            FString PropName = Property->GetName();
            
            if (PropName.Contains(TEXT("CanDash")) || PropName.Contains(TEXT("Can Dash")))
            {
                if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Property))
                {
                    bCanDashFromBP = BoolProp->GetPropertyValue_InContainer(Player);
                    break;
                }
            }
        }
    }
    
    // 실제 Can Dash 상태에 따라 UI 업데이트
    static bool bPreviousCanDash = true;
    static bool bFirstRun = true;
    
    // 첫 실행 시 초기화
    if (bFirstRun)
    {
        bFirstRun = false;
        bPreviousCanDash = bCanDashFromBP;
    	
        if (PlayerUIWidget)
        {
            UWidget* DashWidget = PlayerUIWidget->GetWidgetFromName(TEXT("DashText"));
            if (UTextBlock* DashText = Cast<UTextBlock>(DashWidget))
            {
                DashText->SetText(FText::FromString(TEXT("Dash Ready!")));
                DashText->SetColorAndOpacity(FLinearColor::Green);
            }
        }
        return;
    }
    
    // UI 업데이트 (블루프린트 상태 기반)
    if (PlayerUIWidget)
    {
        UWidget* DashWidget = PlayerUIWidget->GetWidgetFromName(TEXT("DashText"));
        if (UTextBlock* DashText = Cast<UTextBlock>(DashWidget))
        {
            if (bCanDashFromBP)
            {
                DashText->SetText(FText::FromString(TEXT("Dash Ready!")));
                DashText->SetColorAndOpacity(FLinearColor::Green);
            }
            else
            {
                DashText->SetText(FText::FromString(TEXT("Dash CoolTime!")));
                DashText->SetColorAndOpacity(FLinearColor::Red);
            }
        }
    }
    
	bPreviousCanDash = bCanDashFromBP;
}