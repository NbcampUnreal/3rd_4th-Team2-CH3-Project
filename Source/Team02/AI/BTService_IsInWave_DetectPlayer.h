#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_IsInWave_DetectPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TEAM02_API UBTService_IsInWave_DetectPlayer : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_IsInWave_DetectPlayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
