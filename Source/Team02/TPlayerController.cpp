#include "TPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/UserWidget.h"
#include "Character/TCharacterBase.h"
#include "Components/ProgressBar.h"

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