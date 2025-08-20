#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_StartWave.generated.h"

/**
 * 
 */
UCLASS()
class TEAM02_API UBTDecorator_StartWave : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_StartWave();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
