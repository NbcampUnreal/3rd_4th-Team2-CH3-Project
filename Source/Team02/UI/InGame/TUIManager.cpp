#include "TUIManager.h"
#include "TPlayerUIWidget.h"
#include "Character/TCharacterBase.h"
#include "Item/TWeaponBase.h"
#include "Character/TPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"

void UTUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	//테스트 로그
	UE_LOG(LogTemp,Warning,TEXT("UIManager Initialized!!"));
}

void UTUIManager::CreatePlayerUI()
{
	// ⭐ PlayerUIWidgetClass가 설정되지 않았으면 직접 로드
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



void UTUIManager::UpdateAllUI()
{
	// update player weapon
	if (ATPlayerCharacter* PC=Cast<ATPlayerCharacter>(PlayerCharacter))
	{
		CurrentWeapon=PC->CurrentWeapon;
	}
	// update all player ui
	UpdatePlayerHP();
	UpdatePlayerAmmo();

	// 자체 웨이브 정보 업데이트
	if (PlayerUIWidget)
	{
		FString TimeString = GetFormattedTime();
		PlayerUIWidget->UpdateWaveTime(TimeString);
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
