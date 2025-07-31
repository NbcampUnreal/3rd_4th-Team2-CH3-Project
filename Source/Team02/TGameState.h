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
	FTimerHandle HUDUdpateTimerHandle; //UI 업데이트용

	void StartLevel();
	void EndLevel();
	void UpdateHUD();
	void UpdateGameTime();
	
};
