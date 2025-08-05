#include "TUIManager.h"
#include "TPlayerUIWidget.h"
#include "Character/TCharacterBase.h"
#include "Item/TWeaponBase.h"
#include "Character/TPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Area/TCapturePoint.h"
#include "EngineUtils.h"

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

			// 웨이브 시스템 시작
			bWaveActive = true;
			GetWorld()->GetTimerManager().SetTimer(
				WaveTimerHandle,
				this,
				&UTUIManager::UpdateWaveTime,
				1.0f,
				true);

			// UI 업데이트 타이머
			GetWorld()->GetTimerManager().SetTimer(
				UIUpdateTimerHandle,
				this,
				&UTUIManager::UpdateAllUI,
				0.1f,
				true);
			
			// 거점 자동 검색 및 등록(UI 생성 이후)
			FindAndRegisterCapturePoints();

			// 임무 상태 초기화
			UpdateMissionState();
			
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
		PlayerUIWidget->UpdateAmmoInfo(CurrentWeapon->CurrentAmmo,CurrentWeapon->MaxAmmo);
	}
}

void UTUIManager::UpdateWaveInfo(const FString& TimeString,int32 InCurrentWave,int32 InMaxWave)
{
	if (PlayerUIWidget)
	{
		PlayerUIWidget->UpdateWaveTime(TimeString);
	}
}

void UTUIManager::UpdateWaveTime()
{
	if (!bWaveActive) return;

	WaveTime-=1.0f;
	
	if (WaveTime<=0.0f)
	{
		bWaveActive=false;
	}
}

FString UTUIManager::GetFormattedTime() const
{
	int32 Minutes = FMath::FloorToInt(WaveTime / 60.0f);
	int32 Seconds = FMath::FloorToInt(WaveTime) % 60;
	return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
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
void UTUIManager::IncrementMonsterKill()
{
	MonsterKillCount++; //UI용 킬카운트
	RemainingMonsters--; // 웨이브 완료조건
	
	UE_LOG(LogTemp, Warning, TEXT("Monster killed! Total: %d, Remaining: %d"), MonsterKillCount, RemainingMonsters);

	//웨이브 완료 확인
	if (RemainingMonsters<=0 && !bWaveCompleted)
	{
		bWaveCompleted=true;
		bWaveActive=false;
		UE_LOG(LogTemp, Warning, TEXT("Wave completed! All enemies eliminated."));
	}

	UpdateMissionProgress();
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




void UTUIManager::UpdateAllUI()
{
	// 기존 UI 업데이트
	if (ATPlayerCharacter* PC=Cast<ATPlayerCharacter>(PlayerCharacter))
	{
		CurrentWeapon=PC->CurrentWeapon;
	}
	
	UpdatePlayerHP();
	UpdatePlayerAmmo();

	// 자체 웨이브 정보 업데이트
	if (PlayerUIWidget)
	{
		FString TimeString = GetFormattedTime();
		PlayerUIWidget->UpdateWaveTime(TimeString);
	}
	
	// ⭐ 거점 상태 감시 및 UI 업데이트 (디버깅 로그 추가)
	if (CurrentCapturePoint && PlayerUIWidget)
	{
		// ⭐ 디버깅 로그 추가
		if (CurrentCapturePoint->bPlayerInArea)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player is in capture area! Progress: %.1f%%"), CurrentCapturePoint->CapturePercent);
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
		GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
	}
	Super::Deinitialize();
}
