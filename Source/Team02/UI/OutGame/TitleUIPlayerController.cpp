// TitleUIPlayerController.cpp

#include "TitleUIPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"


// ATitleUIPlayerController::ATitleUIPlayerController()
// 	: MainMenuWidgetInstance(nullptr),
// 	  MainMenuWidgetClass(nullptr)
// {
// }

void ATitleUIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FString CurrentMapName = GetWorld()->GetMapName();
	CurrentMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
	else
	{
		// 게임 입력 모드 복원
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
}

void ATitleUIPlayerController::ShowMainMenu(bool bIsRestart)
{
	// 기존 UI 제거
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	// 새 UI 생성
	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			// UI 입력 모드 활성화
			SetInputMode(FInputModeUIOnly());
			bShowMouseCursor = true;
		}

	}
}

void ATitleUIPlayerController::StartGame()
{
	// 게임 전환 전 입력 모드 정리
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;

	UGameplayStatics::OpenLevel(GetWorld(), FName("TitleLevel"));
}