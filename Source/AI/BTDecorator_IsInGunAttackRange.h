#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsInGunAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class TEAM02_API UBTDecorator_IsInGunAttackRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_IsInGunAttackRange();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	//공격 범위
	static const float AttackRange;
};
