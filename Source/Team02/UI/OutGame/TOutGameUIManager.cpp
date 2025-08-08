// TOutGameUIManager.cpp

#include "TOutGameUIManager.h"
#include "UI/InGame/TUIManager.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

AOutGameUIManager::AOutGameUIManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AOutGameUIManager::BeginPlay()
{
    Super::BeginPlay();

    // Restart 대비 UI 및 상태 초기화
    ResetUIAndState();

    // UIManager 이벤트 구독
    SubscribeToUIManagerEvents();

    UE_LOG(LogTemp, Warning, TEXT("OutGameUIManager: Ready to receive events!"));
}

void AOutGameUIManager::ResetUIAndState()
{
    // 이전 UI 제거
    if (VictoryWidget)
    {
        VictoryWidget->RemoveFromParent();
        VictoryWidget = nullptr;
    }
    if (GameOverWidget)
    {
        GameOverWidget->RemoveFromParent();
        GameOverWidget = nullptr;
    }

    // Pause 상태 해제
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        PC->SetPause(false);
        PC->bShowMouseCursor = false;
    }
}

void AOutGameUIManager::SubscribeToUIManagerEvents()
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UTUIManager* UIManager = GI->GetSubsystem<UTUIManager>())
        {
            // 중복 구독 방지
            UIManager->OnVictoryEvent.RemoveDynamic(this, &AOutGameUIManager::ShowVictoryScreen);
            UIManager->OnGameOverEvent.RemoveDynamic(this, &AOutGameUIManager::ShowGameOverScreen);

            // 이벤트 등록
            UIManager->OnVictoryEvent.AddDynamic(this, &AOutGameUIManager::ShowVictoryScreen);
            UIManager->OnGameOverEvent.AddDynamic(this, &AOutGameUIManager::ShowGameOverScreen);

            UE_LOG(LogTemp, Warning, TEXT("OutGameUIManager: Subscribed to UIManager events!"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("OutGameUIManager: Failed to get UIManager!"));
        }
    }
}

void AOutGameUIManager::ShowVictoryScreen()
{
    UE_LOG(LogTemp, Warning, TEXT("OutGameUIManager: Victory event received!"));

    if (VictoryWidgetClass && GetWorld())
    {
        VictoryWidget = CreateWidget<UUserWidget>(GetWorld(), VictoryWidgetClass);
        if (VictoryWidget)
        {
            VictoryWidget->AddToViewport(999); // 최상위 레이어

            if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
            {
                PC->SetPause(true);
                PC->bShowMouseCursor = true;
            }

            UE_LOG(LogTemp, Warning, TEXT("OutGameUIManager: Victory screen displayed!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OutGameUIManager: VictoryWidgetClass is not set!"));
    }
}

void AOutGameUIManager::ShowGameOverScreen()
{
    UE_LOG(LogTemp, Warning, TEXT("OutGameUIManager: GameOver event received!"));

    if (GameOverWidgetClass && GetWorld())
    {
        GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
        if (GameOverWidget)
        {
            GameOverWidget->AddToViewport(999);

            if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
            {
                PC->SetPause(true);
                PC->bShowMouseCursor = true;
            }

            UE_LOG(LogTemp, Warning, TEXT("OutGameUIManager: GameOver screen displayed!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OutGameUIManager: GameOverWidgetClass is not set!"));
    }
}
