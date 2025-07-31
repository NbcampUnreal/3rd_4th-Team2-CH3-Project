#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GunAttack.generated.h"

/**
 * 
 */
UCLASS()
class TEAM02_API UBTTask_GunAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GunAttack();

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
