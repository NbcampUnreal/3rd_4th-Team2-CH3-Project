// TGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TGameMode.generated.h"
class ATEnemySpawner;
UCLASS()
class TEAM02_API ATGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATGameMode();

	// 웨이브 관련
	UPROPERTY(BlueprintReadOnly, Category="Wave")
	int32 CurrentWave = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wave")
	int32 MaxWave = 2;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Spawner")
	TArray<class ATEnemySpawner*> EnemySpawners;
	
	UPROPERTY(BlueprintReadOnly, Category="Wave")
	bool bIsWaveActive = false;

	UFUNCTION(BlueprintCallable, Category="Wave")
	void StartWave(int32 WaveIndex);

	UFUNCTION(BlueprintCallable, Category="Wave")
	void EndWave();

	// (선택) 웨이브 시작을 외부에서 호출할 수 있도록 함수
	UFUNCTION(BlueprintCallable, Category="Wave")
	bool CanStartNextWave() const;
	
	
};
