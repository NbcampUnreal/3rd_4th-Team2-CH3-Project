// TGameState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TGameState.generated.h"

UCLASS()
class TEAM02_API ATGameState : public AGameState
{
	GENERATED_BODY()

public:
	ATGameState();
	virtual void BeginPlay() override;
	
	// 게임 시작 (초 단위)

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Time")
	float GameTime;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Time")
	float MaxGameTime;

	//타이머
	FTimerHandle GameTimerHandle; // 게임시간 감소용
	

	UFUNCTION(BlueprintCallable, Category="Game State")
	void GameOver();
	UFUNCTION(BlueprintCallable, Category="Game State")
	void GameClear();

	void GameStart();
	void GameEnd();
	void UpdateHUD();
	void UpdateGameTime();
	
	// 탈환지와 관련된 코드 필요

	// 레드존과 관련된 코드 필요
};
