// TGameMode.cpp

#include "TGameMode.h"
#include "TPlayerController.h"
#include "Engine/World.h"
#include "Character/TCharacterBase.h"
#include "Spawner/TEnemySpawner.h"

ATGameMode::ATGameMode()
{
	PlayerControllerClass = ATPlayerController::StaticClass();
	DefaultPawnClass = ATCharacterBase::StaticClass();

	CurrentWave = 0;
	MaxWave = 1;
	bIsWaveActive = false;
}

void ATGameMode::StartWave(int32 WaveIndex)
{
	CurrentWave = WaveIndex;

	UE_LOG(LogTemp, Log, TEXT("Wave %d Start!"), WaveIndex);

	for (int32 i = 0; i < EnemySpawners.Num(); ++i)
	{
		if (EnemySpawners[i])
		{
			if (i == WaveIndex)
				EnemySpawners[i]->ActivateSpawner();
			else
				EnemySpawners[i]->DeactivateSpawner();
		}
	}
}


void ATGameMode::EndWave()
{
	if (!bIsWaveActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("No wave is active."));
		return;
	}

	bIsWaveActive = false;
	UE_LOG(LogTemp, Log, TEXT("Wave %d Ended!"), CurrentWave);

	// 예시: 보상 지급, UI 업데이트 등
	// ...
}

bool ATGameMode::CanStartNextWave() const
{
	return !bIsWaveActive && CurrentWave < MaxWave;
}