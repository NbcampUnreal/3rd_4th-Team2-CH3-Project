// TGameMode.cpp

#include "TGameMode.h"
#include "TPlayerController.h"
#include "Engine/World.h"
#include "Character/TCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Spawner/TEnemySpawner.h"
#include "Area/TCapturePoint.h"

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
}

void ATGameMode::StartWave(int32 InWaveIndex)
{
	if (bIsWaveActive) return;  // 이미 웨이브 중

	bIsWaveActive = true;
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
}

void ATGameMode::OnCapturePointCompleted()
{
	bIsWaveActive = false;
	EndWave(); // 현재 웨이브 종료
	++WaveIndex;
}

void ATGameMode::OnZoneOverlap(int32 ZoneIndex)
{
	if (ZoneIndex == WaveIndex)
	{
		StartWave(WaveIndex);
	}
	
}

void ATGameMode::RespawnPlayer(AController* DeadController)
{
	if (LastCapturedPoint)
	{
		// 1. 현재 Pawn 제거
		if (APawn* Pawn = DeadController->GetPawn())
		{
			Pawn->Destroy();
		}

		// 2. Respawn 위치 세팅
		FTransform RespawnTransform = LastCapturedPoint->RespawnTransform;

		// 3. 새로운 Pawn(캐릭터) 스폰
		APawn* NewPawn = SpawnDefaultPawnAtTransform(DeadController, RespawnTransform);

		// 4. 컨트롤러가 새 Pawn을 Possess
		DeadController->Possess(NewPawn);
	}
	else
	{
		// 디폴트 리스폰 (예: 맵 시작 위치)
		RestartPlayer(DeadController);
	}
}