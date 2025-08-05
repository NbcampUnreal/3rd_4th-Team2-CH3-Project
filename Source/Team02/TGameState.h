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
	
	UFUNCTION(BlueprintCallable, Category="Game State")
	void GameOver();
	UFUNCTION(BlueprintCallable, Category="Game State")
	void GameClear();

	void GameStart();
	void GameEnd();
	void UpdateHUD();
	// 탈환지와 관련된 코드 필요
	// 레드존과 관련된 코드 필요
};
