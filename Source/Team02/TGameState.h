// TGameState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameTimeUpdate,float,GameTime);

UCLASS()
class TEAM02_API ATGameState : public AGameState
{
	GENERATED_BODY()

public:
	ATGameState();
	virtual void BeginPlay() override;

	// 게임 시간 델리게이트
	UPROPERTY(BlueprintAssignable,Category="Time")
	FOnGameTimeUpdate OnGameTimeUpdate;
	
	//웨이브 관련 변수(웨이브 시간등등)
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Wave")
	float WaveTime=300.f;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Wave")
	int32 CurrentWave=1;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Wave")
	int32 MaxWave=2;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Wave")
	bool bWaveActive=false;

	//시간 분초로 바꾸는 함수
	UFUNCTION(BlueprintCallable,Category="Time")
	FString GetFormattedTime() const;

	

	//타이머
	FTimerHandle GameTimerHandle; // 게임시간 감소용
	

	UFUNCTION(BlueprintCallable, Category="Game State")
	void GameOver();
	UFUNCTION(BlueprintCallable, Category="Game State")
	void GameClear();

	void GameStart();
	void GameEnd();
	void UpdateHUD();
	void UpdateWaveTime(); //웨이브 타임 함수
	void WaveTimeUp(); // 웨이브 종료
	
	// 탈환지와 관련된 코드 필요

	// 레드존과 관련된 코드 필요
};
