// TGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TGameMode.generated.h"

class ATEnemySpawner;
class ATCapturePoint;
class ATAIController;
class ATSwordAIController;

UCLASS()
class TEAM02_API ATGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATGameMode();

	// 게임 재시작 함수 오버라이드 (UFUNCTION 매크로 제거)
	virtual void RestartGame() override;

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

	// 사망 처리
	UFUNCTION()
	void OnPlayerDied(AController* DeadController);

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	// Blueprint에서 호출할 수 있는 별도 함수
	UFUNCTION(BlueprintCallable, Category = "Game")
	void RestartGameFromUI();
	
	//AI 컨트롤러를 베열에 추가하는 함수
	void RegisterAIController(ATAIController* AIController);

	void UnregisterAIController(ATAIController* AIController);

	void RegisterAISwordController(ATSwordAIController* AIController);

	void UnregisterAISwordController(ATSwordAIController* AIController);

	TArray<AActor*> FoundActors;

	UPROPERTY()
	TArray<TObjectPtr<ATAIController>> AIControllers;

	UPROPERTY()
	TArray<TObjectPtr<ATSwordAIController>> SwordAIControllers;

private:
	// 게임 상태 초기화 함수들
	void InitializeGameState();
	void InitializeUIManager();
};
