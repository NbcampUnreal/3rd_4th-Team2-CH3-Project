#include "TPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/UserWidget.h"

ATPlayerController::ATPlayerController()
{
}
void ATPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerUIWidgetClass)
	{
		UUserWidget* PlayerUIWidget = CreateWidget<UUserWidget>(this, PlayerUIWidgetClass);
			if (PlayerUIWidget)
			{
				PlayerUIWidget->AddToViewport();
			}
	}
}

void ATPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}