// TGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TGameMode.generated.h"

class ATEnemySpawner;
class ATCapturePoint;
class ATBossSpawner;
class ATAIController;

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
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void RespawnPlayer(AController* DeadController);

	// 사망 처리
	UFUNCTION()
	void OnPlayerDied(AController* DeadController);

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	ATBossSpawner* BossSpawner;
	//AI 컨트롤러를 베열에 추가하는 함수
	void RegisterAIController(ATAIController* AIController);

	void UnregisterAIController(ATAIController* AIController);

	TArray<AActor*> FoundActors;

	UPROPERTY()
	TArray<TObjectPtr<ATAIController>> AIControllers;
};
