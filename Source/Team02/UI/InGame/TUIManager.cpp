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
		PlayerUIWidget->UpdateAmmoInfo(CurrentWeapon->GetCurrentAmmo(),CurrentWeapon->MaxAmmo);
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
	if (UWorld* World=GetWorld())
	{
		//test log
		UE_LOG(LogTemp, Warning, TEXT("Searching for capture points in the level..."));
		//월드에서 모든 Capturepoint 찾기
		for (TActorIterator<ATCapturePoint> ActorItr(World);ActorItr; ++ActorItr)
		{
			ATCapturePoint* CapturePoint= *ActorItr;
			if (CapturePoint)
			{
				RegisterCapturePoint(CapturePoint);

				// 거점 이름 설정(엑터,이름 사용)
				CapturePointName=CapturePoint->GetName();

				UE_LOG(LogTemp, Warning, TEXT("Found and registered capture point: %s"), *CapturePointName);

				// 첫쨰 거전만 등록(여러 거점 있을시 break 제거)
				break;
			}
		}
		if (!CurrentCapturePoint)
		{
			UE_LOG(LogTemp,Warning,TEXT("No capture points found in level"));
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
	//게임플로우-> 웨이브 시작> 몹처치>점령지 탈환> 무기 해금 > 보스전
	if (bBossPhase)
	{
		NewObjective=TEXT("Defeat the boss");
	}
	else if (bCaptureCompleted && bWeaponUnlocked)
	{
		//점령 완료 + 무기 해금 후
		NewObjective=TEXT("New Weapon unlocked! Prepare for boss");
	}
	else if (bCapturePhase && bNearCapturePoint)
	{
		//웨이브 완료후 거점 근처
		NewObjective=TEXT("Capture the control point");
	}
	else if (bWaveCompleted && !bCapturePhase)
	{
		// 웨이브 완료했으나 아직 거점으로 이동 안함
		NewObjective=TEXT("Move to control point");
		bCapturePhase=true; // 점령 페이즈 활성화
	}
	else if (bWaveActive)
	{
		//웨이브 진행중
		NewObjective=FString::Printf(TEXT("Eliminate enemies(%d remaining)"),RemainingMonsters);
	}
	else
	{
		//기본 상태
		NewObjective=TEXT("Eliminate all enemies");
	}

	// 임무 변경시에만 업데이트
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

	int32 CurrentMonsterCount=TrackedMonsters.Num();
	int32 CurrentBossCount=TrackedBosses.Num();

	//디버깅 로그
	if (bBossPhase)
	{
		UE_LOG(LogTemp,Warning,TEXT("Boss Phase Active! Boss Count: %d, Last Count: %d"),
			CurrentBossCount,LastFrameBossCount);
	}
	
	// 웨이브가 활성화 되어있을때만 처치 카운트 
	if (CurrentMonsterCount< LastFrameMonsterCount && bWaveActive && !bWaveCompleted)
	{
		int32 KilledCount=LastFrameMonsterCount-CurrentMonsterCount;

		for (int32 i=0; i<KilledCount; i++)
		{
			MonsterKillCount++;
			RemainingMonsters--;

			UE_LOG(LogTemp,Warning,TEXT("Monster detected as killed!! Total: %d, Remaining: %d"),
				MonsterKillCount,RemainingMonsters);
		}
		
		// UI 업데이트
		if (PlayerUIWidget)
		{
			int32 TotalMonsters=MonsterKillCount + RemainingMonsters;
			PlayerUIWidget->UpdateKillCount(MonsterKillCount,TotalMonsters);
		}
		
		// wave 완료 체크
		if (RemainingMonsters<=0 && !bWaveCompleted)
		{
			bWaveCompleted=true;
			bWaveActive=false;
			UE_LOG(LogTemp,Warning,TEXT("Wave completed! All enemies elminated.!"));
		}
		
		UpdateMissionProgress();
	}

	// 보스 처치 감지 (별도 분리, 보스페이즈에서만)
	if (bBossPhase && CurrentBossCount<LastFrameBossCount)
	{
		UE_LOG(LogTemp,Warning,TEXT("Boss DEFEATED!! Game Complete!"));

		//게임 완료 처리
		bBossPhase=false;
		SetMissionObjective(TEXT("Victory! Boss Defeated!"));

		// 승리 이벤트 발생!
		OnVictoryEvent.Broadcast();
		UE_LOG(LogTemp,Warning,TEXT("Victory event broadcasted!!"));


		
	}

	
	// 스포너 상태 체크로 웨이브 활성화 감지
	bool bAnySpawnerActive=false;
	for (ATEnemySpawner* Spawner: RegisteredSpawners)
	{
		if (Spawner && Spawner->GetCurrentSpawned()>0)
		{
			bAnySpawnerActive=true;
			break;
		}
	}

	// 웨이브 시작 감지
	if (bAnySpawnerActive && !bWaveActive && !bWaveCompleted)
	{
		// 새 웨이브 시작
		bWaveActive=true;
		bWaveCompleted=false;
		UpdateWaveInfoFromSpawners();
		UE_LOG(LogTemp,Warning,TEXT("New wave started!"));

		//웨이브 시작 알림 부분
		if (PlayerUIWidget)
		{
			PlayerUIWidget->ShoWEnemyIncomingAlarm();

			//3초후 알람 숨김
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
		UE_LOG(LogTemp,Warning,TEXT("New Wave started!"));
	}

	LastFrameMonsterCount=CurrentMonsterCount;
	LastFrameBossCount=CurrentBossCount;
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






void UTUIManager::UpdateAllUI()
{
	// 기존 UI 업데이트
	if (ATPlayerCharacter* PC=Cast<ATPlayerCharacter>(PlayerCharacter))
	{
		CurrentWeapon=PC->CurrentWeapon;
	}
	
	UpdatePlayerHP();
	UpdatePlayerAmmo();
	
	// 거점 상태 감시 및 UI 업데이트 (디버깅 로그 추가)
	if (CurrentCapturePoint && PlayerUIWidget)
	{
		// 디버깅 로그 추가
		if (CurrentCapturePoint->bPlayerInArea)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player is in capture area! Progress: %.1f%%"),CurrentCapturePoint->CapturePercent);
		}
        
		// 플레이어가 거점 안에 있는지 확인
		if (CurrentCapturePoint->bPlayerInArea)
		{
			// UI가 아직 안 보이면 표시
			if (!PlayerUIWidget->IsCaptureUIVisible())
			{
				UE_LOG(LogTemp, Warning, TEXT("Showing capture UI"));
				ShowCaptureUI(CapturePointName);
			}
            
			// 점령률 업데이트 (0-100 → 0.0-1.0 변환)
			float ProgressPercent = CurrentCapturePoint->CapturePercent / 100.0f;
			UpdateCaptureProgress(ProgressPercent);

			// 점령 완료 감지 추가
			if (CurrentCapturePoint->CapturePercent>=100.0f && !bCaptureCompleted)
			{
				bCaptureCompleted=true;
				bBossPhase=true;
				UE_LOG(LogTemp,Warning,TEXT("Capture completed! Boss phase activated!"));
				UpdateMissionState();
			}
		}
		
		else
		{
			// 플레이어가 거점 밖에 있으면 UI 숨김
			if (PlayerUIWidget->IsCaptureUIVisible())
			{
				UE_LOG(LogTemp, Warning, TEXT("Hiding capture UI"));
				HideCaptureUI();
			}
		}
	}
	else
	{
		// ⭐ 문제 진단 로그
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
