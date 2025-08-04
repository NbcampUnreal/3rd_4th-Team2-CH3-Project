#include "TPlayerController.h"
#include "TGameState.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/UserWidget.h"
#include "Character/TCharacterBase.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


ATPlayerController::ATPlayerController()
{
}
void ATPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerUIWidgetClass)
	{
		PlayerUIWidget = CreateWidget<UUserWidget>(this, PlayerUIWidgetClass);
			if (PlayerUIWidget)
			{
				PlayerUIWidget->AddToViewport();

				// 타이머 시작
				GetWorldTimerManager().SetTimer(
					UIUpdateTimerHandle,
					this,
					&ATPlayerController::UpdateHPBar,
					0.1f,
					true
					);

				if (ATGameState* GS=GetWorld()->GetGameState<ATGameState>())
				{
					GS->OnGameTimeUpdate.AddDynamic(this,&ATPlayerController::OnGameTimeUpdate);
				}
				
			}
	}
}

void ATPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ATPlayerController::OnGameTimeUpdate(float NewTime)
{
	if (!PlayerUIWidget) return;

	//Game State에서 시간 포맷형 함수사용
	if (ATGameState* GS=GetWorld()->GetGameState<ATGameState>())
	{
		
		// Wave Time Text 업데이트
		FString TimeString = GS->GetFormattedTime();
		UTextBlock* TimeText=Cast<UTextBlock>(PlayerUIWidget->GetWidgetFromName(TEXT("WaveTimeText")));
		if (TimeText)
		{
			TimeText->SetText(FText::FromString(TimeString));
		}
		// Wave Level Text 업데이트
		FString WaveLevelString=FString::Printf(TEXT("Wave %d/%d"),GS->CurrentWave,GS->MaxWave);
		UTextBlock* WaveLevelText=Cast<UTextBlock>(PlayerUIWidget->GetWidgetFromName(TEXT("WaveLevelText")));
		if (WaveLevelText)
		{
			WaveLevelText->SetText(FText::FromString(WaveLevelString));
		}
	}
}


//플레이어 체력바 업데이트 관련 함수
void ATPlayerController::UpdateHPBar()
{
	if (!PlayerUIWidget) return;

	ATCharacterBase* PlayerCharacter=Cast<ATCharacterBase>(GetPawn());
	if (!PlayerCharacter) return;

	//ProgressBar 업데이트
	UProgressBar* HPBar=Cast<UProgressBar>(PlayerUIWidget->GetWidgetFromName(TEXT("HPBar")));
	if (HPBar)
	{
		float HPPercent=PlayerCharacter->GetCurrentHP()/PlayerCharacter->GetMaxHP();
		HPBar->SetPercent(HPPercent);
	}
	//HPText 업데이트
	UTextBlock* HPText=Cast<UTextBlock>(PlayerUIWidget->GetWidgetFromName(TEXT("HPText")));
	if (HPText)
	{
		FString HPString=FString::Printf(TEXT("%.0f/%.0f"),
			PlayerCharacter->GetCurrentHP(),
			PlayerCharacter->GetMaxHP());
			HPText->SetText(FText::FromString(HPString));
	}
	
}
