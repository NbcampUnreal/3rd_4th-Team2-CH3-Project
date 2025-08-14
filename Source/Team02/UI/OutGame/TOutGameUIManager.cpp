// TOutGameUIManager.cpp

#include "TOutGameUIManager.h"
#include "UI/InGame/TUIManager.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"    

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

    // 지연된 이벤트 구독 (UIManager가 완전히 초기화된 후)
    FTimerHandle DelayedSubscribeTimer;
    GetWorld()->GetTimerManager().SetTimer(
        DelayedSubscribeTimer,
        this,
        &AOutGameUIManager::SubscribeToUIManagerEvents,
        1.0f, // 1초 지연
        false
    );

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
            // 기존 구독 완전히 해제
            UIManager->OnVictoryEvent.RemoveAll(this);
            UIManager->OnGameOverEvent.RemoveAll(this);

            // 이벤트 등록
            UIManager->OnVictoryEvent.AddDynamic(this, &AOutGameUIManager::ShowVictoryScreen);
            UIManager->OnGameOverEvent.AddDynamic(this, &AOutGameUIManager::ShowGameOverScreen);

            UE_LOG(LogTemp, Warning, TEXT("OutGameUIManager: Subscribed to UIManager events!"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("OutGameUIManager: Failed to get UIManager!"));

            // 재시도 로직
            FTimerHandle RetryTimer;
            GetWorld()->GetTimerManager().SetTimer(
                RetryTimer,
                this,
                &AOutGameUIManager::SubscribeToUIManagerEvents,
                2.0f,
                false
            );
            
        }
    }
}


// UIManager 이벤트 구독 해제
void AOutGameUIManager::UnsubscribeFromUIManagerEvents()
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UTUIManager* UIManager = GI->GetSubsystem<UTUIManager>())
        {
            UIManager->OnVictoryEvent.RemoveDynamic(this, &AOutGameUIManager::ShowVictoryScreen);
            UIManager->OnGameOverEvent.RemoveDynamic(this, &AOutGameUIManager::ShowGameOverScreen);

            UE_LOG(LogTemp, Warning, TEXT("OutGameUIManager: Unsubscribed from UIManager events!"));
        }
    }
}

// Restart 시 호출할 함수 (UI 제거 + 이벤트 재구독)
void AOutGameUIManager::HandleRestart()
{
    UnsubscribeFromUIManagerEvents(); // 이전 이벤트 구독 해제
    ResetUIAndState();                 // UI와 상태 초기화
    SubscribeToUIManagerEvents();      // 새 라운드용 이벤트 재구독
}

void AOutGameUIManager::ShowVictoryScreen()
{
    UE_LOG(LogTemp, Warning, TEXT("OutGameUIManager: Victory event received!"));

    // 중복 실행 방지
    if (VictoryWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("OutGameUIManager: Victory screen already showing!"));
        return;
    }
    
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

    // 중복 실행 방지
    if (GameOverWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("OutGameUIManager: GameOver screen already showing!"));
        return;
    }
    
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

// 명시적 정리 함수
void AOutGameUIManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UE_LOG(LogTemp, Warning, TEXT("OutGameUIManager: EndPlay called"));
    
    // 이벤트 구독 해제
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UTUIManager* UIManager = GI->GetSubsystem<UTUIManager>())
        {
            UIManager->OnVictoryEvent.RemoveAll(this);
            UIManager->OnGameOverEvent.RemoveAll(this);
            UE_LOG(LogTemp, Warning, TEXT("OutGameUIManager: Events unsubscribed"));
        }
    }
    
    // UI 정리
    ResetUIAndState();
    
    Super::EndPlay(EndPlayReason);
}

// Quit 버튼용 함수
void AOutGameUIManager::QuitToMainMenu()
{
    UE_LOG(LogTemp, Warning, TEXT("OutGameUIManager: Quit to main menu requested"));
    
    // 현재 레벨 이름 저장 (다시 시작할 때 사용)
    FString CurrentLevelName = GetWorld()->GetMapName();
    CurrentLevelName = FPackageName::GetShortName(CurrentLevelName);
    
    // 메인 메뉴로 이동
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("MenuLevel"));
    
}