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

	void StartLevel();
	void EndLevel();
	void UpdateHUD();
	
};
