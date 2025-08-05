// TiTleUIPlayerController.cpp

#include "TitleUIPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

ATitleUIPlayerController::ATitleUIPlayerController()
	: MainMenuWidgetInstance(nullptr),
	  MainMenuWidgetClass(nullptr)
{
}

void ATitleUIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 게임 실행 시 메뉴 레벨에서 메뉴 UI 먼저 표시
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
}

// 메뉴 UI 표시
void ATitleUIPlayerController::ShowMainMenu(bool bIsRestart)
{
	// HUD가 켜져 있다면 닫기
	// if (HUDWidgetInstance)
	// {
	// 	HUDWidgetInstance->RemoveFromParent();
	// 	HUDWidgetInstance = nullptr;
	// }

	// 이미 메뉴가 떠 있으면 제거
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	// 메뉴 UI 생성
	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("StartButton")))
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString("Restart"));
			}
			else
			{
				ButtonText->SetText(FText::FromString("Start"));
			}
		}
	}
}

// 게임 시작 - PlayGround 오픈
void ATitleUIPlayerController::StartGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("TitleLevel"));
}


