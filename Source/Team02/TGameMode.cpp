// TGameMode.cpp

#include "TGameMode.h"
#include "TPlayerController.h"
#include "Engine/World.h"
#include "Character/TCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Spawner/TEnemySpawner.h"
#include "Area/TCapturePoint.h"
#include "Spawner/TBossSpawner.h"
#include "AI/TAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"
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
	
	EnemySpawners.Empty();
	for (AActor* Actor : FoundActors)
	{
		if (ATEnemySpawner* Spawner = Cast<ATEnemySpawner>(Actor))
		{
			EnemySpawners.Add(Spawner);
		}
	}

	TArray<AActor*> FoundBossSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATBossSpawner::StaticClass(), FoundBossSpawners);
	if (FoundBossSpawners.Num() > 0)
	{
		BossSpawner = Cast<ATBossSpawner>(FoundBossSpawners[0]);
		UE_LOG(LogTemp, Warning, TEXT("BossSpawner 할당됨: %s"), *BossSpawner->GetName());
	}
	else
	{
		BossSpawner = nullptr;
		UE_LOG(LogTemp, Error, TEXT("레벨에 BossSpawner 없음!"));
	}
}

void ATGameMode::RegisterAIController(ATAIController* AIController)
{
	if (IsValid(AIController) == true)
	{
		AIControllers.Add(AIController);
	}
}

void ATGameMode::UnregisterAIController(ATAIController* AIController)
{
	if (IsValid(AIController) == true)
	{
		AIControllers.Remove(AIController);
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

	if (WaveIndex == 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Boss Spawned	"));
		BossSpawner -> SpawnBoss();
	}

	UWorld* World = GetWorld();
	
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
}

void ATGameMode::OnCapturePointCompleted()
{
	bIsWaveActive = false;
	
	++WaveIndex;
	EndWave(); // 현재 웨이브 종료
}

void ATGameMode::OnZoneOverlap(int32 ZoneIndex)
{
	if (ZoneIndex == WaveIndex)
	{
		StartWave(WaveIndex);
	}
	
}

// void ATGameMode::RespawnPlayer(AController* DeadController)
// {
// 	if (LastCapturedPoint)
// 	{
// 		// 1. 현재 Pawn 제거
// 		if (APawn* Pawn = DeadController->GetPawn())
// 		{
// 			Pawn->Destroy();
// 		}
//
// 		// 2. Respawn 위치 세팅
// 		FTransform RespawnTransform = LastCapturedPoint->RespawnTransform;
//
// 		// 3. 새로운 Pawn(캐릭터) 스폰
// 		APawn* NewPawn = SpawnDefaultPawnAtTransform(DeadController, RespawnTransform);
//
// 		// 4. 컨트롤러가 새 Pawn을 Possess
// 		DeadController->Possess(NewPawn);
// 	}
// 	else
// 	{
// 		// 디폴트 리스폰 (예: 맵 시작 위치)
// 		RestartPlayer(DeadController);
// 	}
// }

// 플레이어 리스폰
void ATGameMode::RespawnPlayer(AController* DeadController)
{
	if (!DeadController) return;

	// PlayerController로 캐스팅
	if (APlayerController* PC = Cast<APlayerController>(DeadController))
	{
		// 플레이어 입력을 게임 전용 모드로 설정 (UI 입력 차단, 마우스 커서 숨김)
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false; // 커서 숨기기
	}

	// 기존 Pawn 제거
	if (APawn* Pawn = DeadController->GetPawn())
	{
		DeadController->UnPossess();
		Pawn->Destroy();
	}

	// 리스폰 위치 결정
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

	// 새 Pawn 스폰 후 Possess
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

