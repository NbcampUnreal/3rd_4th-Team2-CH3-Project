#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TAIController.generated.h"

/**
 * 
 */
UCLASS()
class TEAM02_API ATAIController : public AAIController
{
	GENERATED_BODY()

public:
	ATAIController();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	//패트롤 
	void OnPatrolTimerElapsed();

public:
	FTimerHandle PatrolTimerHandle = FTimerHandle();

	//순찰 주기
	static const float PatrolRepeatInterval;

	//경계 반지를 크기
	static const float PatrolRadius;

	
};
