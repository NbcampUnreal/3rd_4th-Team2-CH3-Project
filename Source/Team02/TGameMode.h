// TGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TGameMode.generated.h"
class ATEnemySpawner;
class ATCapturePoint;
class ATBossSpawner;
UCLASS()
class TEAM02_API ATGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATGameMode();

	virtual void BeginPlay() override;
	// 웨이브 관련
	UPROPERTY(BlueprintReadOnly, Category="Wave")
	int32 CurrentWave = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave")
	int32 MaxWave = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner")
	TArray<ATEnemySpawner*> EnemySpawners;
	
	UPROPERTY(BlueprintReadOnly, Category="Wave")
	bool bIsWaveActive = false;

	UPROPERTY(BlueprintReadWrite)
	ATCapturePoint* LastCapturedPoint;
	
	UPROPERTY()
	int32 WaveIndex = 0;

	UFUNCTION()
	void StartWave(int32 InWaveIndex);

	UFUNCTION()
	void EndWave();

	UFUNCTION()
	void OnCapturePointCompleted(); // 거점 점령 완료 시 호출 (100% 달성)

	UFUNCTION()
	void OnZoneOverlap(int32 ZoneIndex);
	void RespawnPlayer(AController* DeadController);

	ATBossSpawner* BossSpawner;
	TArray<AActor*> FoundActors;
};
