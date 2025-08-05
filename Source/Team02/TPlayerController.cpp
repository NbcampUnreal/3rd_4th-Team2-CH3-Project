#include "TPlayerController.h"
#include "GameFramework/Pawn.h"
#include "TGameState.h"
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
		// 'UUserWidget*' 타입을 붙이지 않고, 멤버 변수에 대입!
		PlayerUIWidget = CreateWidget<UUserWidget>(this, PlayerUIWidgetClass);
		if (PlayerUIWidget)
		{
			PlayerUIWidget->AddToViewport();
		}
		//시간 델리게이트 관련함수
		if (ATGameState* GS=GetWorld()->GetGameState<ATGameState>())
		{
			//GS->OnGameTimeUpdate.AddDynamic(this,&ATPlayerController::OnGameTimeUpdate);
		}
	}
}

void ATPlayerController::UpdateHPBar()
{
	if (!PlayerUIWidget) return;

	// 플레이어 캐릭터 가져오기
	ATCharacterBase* PlayerCharacter=Cast<ATCharacterBase>(GetPawn());
	if (!PlayerCharacter) return;

	//ProgessBar 찾기(UI에서는 progessbar 이름이 HPBar임
	UProgressBar* HPBar=Cast<UProgressBar>(PlayerUIWidget->GetWidgetFromName(TEXT("HPBar")));
	if (HPBar)
	{
		float HPPercent=PlayerCharacter->GetCurrentHP() / PlayerCharacter->GetMaxHP();
		HPBar->SetPercent(HPPercent);
	}
}

void ATPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ATPlayerController::OnGameTimeUpdate(float NewTime)
{
	if (!PlayerUIWidget) return;

	if (ATGameState* GS = GetWorld()->GetGameState<ATGameState>())
	{
		// 웨이브 시간 업데이트
		//FString TimeString = GS->GetFormattedTime();
		UTextBlock* WaveTimeText = Cast<UTextBlock>(PlayerUIWidget->GetWidgetFromName(TEXT("WaveTimeText")));
		if (WaveTimeText)
		{
			//WaveTimeText->SetText(FText::FromString(TimeString));
		}

		// 웨이브 레벨 업데이트
		//FString WaveLevelString = FString::Printf(TEXT("Wave %d/%d"), GS->CurrentWave, GS->MaxWave);
		UTextBlock* WaveLevelText = Cast<UTextBlock>(PlayerUIWidget->GetWidgetFromName(TEXT("WaveLevelText")));
		if (WaveLevelText)
		{
			//WaveLevelText->SetText(FText::FromString(WaveLevelString));
		}
	}
}