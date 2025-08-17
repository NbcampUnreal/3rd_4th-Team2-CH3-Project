// TGameMode.cpp

#include "TGameMode.h"
#include "TPlayerController.h"
#include "Engine/World.h"
#include "UI/InGame/TUIManager.h"
#include "Engine/GameInstance.h"
#include "Character/TCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Spawner/TEnemySpawner.h"
#include "Area/TCapturePoint.h"
#include "AI/TAIController.h"
#include "Controller/TSwordAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"

ATGameMode::ATGameMode()
{
	PlayerControllerClass = ATPlayerController::StaticClass();
	DefaultPawnClass = ATCharacterBase::StaticClass();

	CurrentWave = 0;
	MaxWave = 1;
	bIsWaveActive = false;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATEnemySpawner::StaticClass(), FoundActors);

	// 2. EnemySpawners에 캐스팅해서 넣기
	
}

void ATGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작 시 항상 모든 상태 초기화
	InitializeGameState();
	
	EnemySpawners.Empty();
	for (AActor* Actor : FoundActors)
	{
		if (ATEnemySpawner* Spawner = Cast<ATEnemySpawner>(Actor))
		{
			EnemySpawners.Add(Spawner);
		}
	}

	// UIManager 초기화 (지연 호출)
	FTimerHandle UIInitTimer;
	GetWorld()->GetTimerManager().SetTimer(
		UIInitTimer,
		this,
		&ATGameMode::InitializeUIManager,
		1.0f, // 1초 지연
		false
	);
}

// 게임 상태 초기화
void ATGameMode::InitializeGameState()
{
	UE_LOG(LogTemp, Warning, TEXT("Initializing Game State..."));
	
	// GameMode 상태 초기화
	CurrentWave = 0;
	MaxWave = 1;
	bIsWaveActive = false;
	WaveIndex = 0;
	LastCapturedPoint = nullptr;
	
	
	// 모든 Enemy Spawner 비활성화
	for (ATEnemySpawner* Spawner : EnemySpawners)
	{
		if (IsValid(Spawner))
		{
			Spawner->SetSpawnerActive(false);
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Game State Initialized!"));
}

// UIManager 초기화
void ATGameMode::InitializeUIManager()
{
	UE_LOG(LogTemp, Warning, TEXT("Initializing UIManager..."));
	
	if (UGameInstance* GI = GetGameInstance())
	{
		UTUIManager* UIManager = GI->GetSubsystem<UTUIManager>();
		if (UIManager)
		{
			UE_LOG(LogTemp, Warning, TEXT("UIManager found, calling RestartGameUI()"));
			UIManager->RestartGameUI();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UIManager not found during initialization!"));
		}
	}
}

void ATGameMode::RegisterAIController(ATAIController* AIController)
{
	if (IsValid(AIController) == true)
	{
		AIControllers.Add(AIController);
	}
}

void ATGameMode::RegisterAISwordController(ATSwordAIController* AIController)
{
	if (IsValid(AIController) == true)
	{
		SwordAIControllers.Add(AIController);
	}
}

void ATGameMode::UnregisterAIController(ATAIController* AIController)
{
	if (IsValid(AIController) == true)
	{
		AIControllers.Remove(AIController);
	}
}

void ATGameMode::UnregisterAISwordController(ATSwordAIController* AIController)
{
	if (IsValid(AIController) == true)
	{
		SwordAIControllers.Remove(AIController);
	}
}



void ATGameMode::StartWave(int32 InWaveIndex)
{
	if (bIsWaveActive) return;  // 이미 웨이브 중
	
	bIsWaveActive = true;

	//AI블랙보드의 키를 true로 전환
	for (ATAIController* AIC : AIControllers)
	{
		if (IsValid(AIC) == true)
		{
			UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(AIC->GetBlackboardComponent());
			if (IsValid(BlackboardComponent) == true)
			{
				BlackboardComponent->SetValueAsBool(AIC->IsInWaveKey, true);
			}
		}
	}

	for (ATSwordAIController* SAIC : SwordAIControllers)
	{
		if (IsValid(SAIC) == true)
		{
			UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(SAIC->GetBlackboardComponent());
			if (IsValid(BlackboardComponent) == true)
			{
				BlackboardComponent->SetValueAsBool(SAIC->SwordIsInWaveKey, true);
			}
		}
	}
	
	if (EnemySpawners.IsValidIndex(InWaveIndex))
	{
		EnemySpawners[InWaveIndex]->SetSpawnerActive(true); // 예시: 스포너 켜기
		// 추가로 웨이브 시작 관련 로직
	}
}


void ATGameMode::EndWave()
{
	
	// 현재 웨이브에 대한 정리 작업(스포너 비활성화 등)
	if (EnemySpawners.IsValidIndex(WaveIndex))
	{
		EnemySpawners[WaveIndex]->SetSpawnerActive(false); // 예시: 활성화 끄기
	}
	
	
	//AI블랙보드의 키를 false로 전환
	for (ATAIController* AIC : AIControllers)
	{
		if (IsValid(AIC) == true)
		{
			UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(AIC->GetBlackboardComponent());
			if (IsValid(BlackboardComponent) == true)
			{
				BlackboardComponent->SetValueAsBool(AIC->IsInWaveKey, false);
			}
		}
	}

	//AI블랙보드의 키를 false로 전환
	for (ATSwordAIController* AIC : SwordAIControllers)
	{
		if (IsValid(AIC) == true)
		{
			UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(AIC->GetBlackboardComponent());
			if (IsValid(BlackboardComponent) == true)
			{
				BlackboardComponent->SetValueAsBool(AIC->SwordIsInWaveKey, false);
			}
		}
	}
}

void ATGameMode::OnCapturePointCompleted()
{
	bIsWaveActive = false;
	EndWave();
	++WaveIndex;
	
}

void ATGameMode::OnZoneOverlap(int32 ZoneIndex)
{
	
	if (ZoneIndex == WaveIndex)
	{
		StartWave(WaveIndex);
	}
	
}

// 플레이어 리스폰
void ATGameMode::RespawnPlayer(AController* DeadController)
{
	if (!DeadController) return;

	// 1. 게임 일시정지 해제
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
	}

	// 2. 입력 모드 게임 전용으로 변경
	if (APlayerController* PC = Cast<APlayerController>(DeadController))
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false; // 커서 숨기기
	}

	// 3. 기존 Pawn 제거
	if (APawn* Pawn = DeadController->GetPawn())
	{
		DeadController->UnPossess();
		Pawn->Destroy();
	}

	// 4. 리스폰 위치 결정
	FTransform RespawnTransform;
	if (LastCapturedPoint)
	{
		RespawnTransform = LastCapturedPoint->RespawnTransform;
	}
	else if (AActor* PlayerStart = FindPlayerStart(DeadController))
	{
		RespawnTransform = PlayerStart->GetActorTransform();
	}
	else
	{
		return;
	}

	// 5. 새 Pawn 스폰 후 Possess
	APawn* NewPawn = SpawnDefaultPawnAtTransform(DeadController, RespawnTransform);
	if (!NewPawn) return;

	DeadController->Possess(NewPawn);
}


// 플레이어 사망
void ATGameMode::OnPlayerDied(AController* DeadController)
{
	if (!DeadController || !GameOverWidgetClass) return;

	APlayerController* PC = Cast<APlayerController>(DeadController);
	if (!PC) return;

	UUserWidget* GameOverUI = CreateWidget<UUserWidget>(PC, GameOverWidgetClass);
	if (GameOverUI)
	{
		GameOverUI->AddToViewport();
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(GameOverUI->TakeWidget());
		PC->SetInputMode(InputMode);
	}
}

// RestartGame 함수 오버라이드
void ATGameMode::RestartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("ATGameMode::RestartGame() override called"));
    
	// 부모 클래스의 RestartGame 호출
	Super::RestartGame();
    
	// 추가 커스텀 로직
	RestartGameFromUI();
}

// Blueprint에서 호출할 수 있는 함수 구현
void ATGameMode::RestartGameFromUI()
{
    UE_LOG(LogTemp, Warning, TEXT("ATGameMode::RestartGameFromUI() called"));
    
    // 1. UIManager 가져오기
    if (UGameInstance* GI = GetGameInstance())
    {
        UTUIManager* UIManager = GI->GetSubsystem<UTUIManager>();
        if (UIManager)
        {
            UE_LOG(LogTemp, Warning, TEXT("UIManager found, calling RestartGameUI()"));
            
            // 2. UIManager의 RestartGameUI 호출
            UIManager->RestartGameUI();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("UIManager not found!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance not found!"));
    }
    
    // 3. GameMode 자체 상태 초기화
    CurrentWave = 0;
    MaxWave = 1;
    bIsWaveActive = false;
    WaveIndex = 0; // WaveIndex도 초기화
    
    // 4. AI Controller들의 상태 초기화
    for (ATAIController* AIC : AIControllers)
    {
        if (IsValid(AIC))
        {
            UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(AIC->GetBlackboardComponent());
            if (IsValid(BlackboardComponent))
            {
                BlackboardComponent->SetValueAsBool(AIC->IsInWaveKey, false);
            }
        }
    }

    for (ATSwordAIController* SAIC : SwordAIControllers)
    {
        if (IsValid(SAIC))
        {
            UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(SAIC->GetBlackboardComponent());
            if (IsValid(BlackboardComponent))
            {
                BlackboardComponent->SetValueAsBool(SAIC->SwordIsInWaveKey, false);
            }
        }
    }
    
    // 5. 모든 Enemy Spawner 비활성화
    for (ATEnemySpawner* Spawner : EnemySpawners)
    {
        if (IsValid(Spawner))
        {
            Spawner->SetSpawnerActive(false);
            UE_LOG(LogTemp, Warning, TEXT("Spawner %s deactivated"), *Spawner->GetName());
        }
    }
    
    // 6. LastCapturedPoint 초기화
    LastCapturedPoint = nullptr;
    
    UE_LOG(LogTemp, Warning, TEXT("🎮 Game restart completed!"));
}