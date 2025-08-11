#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SwordAttack.generated.h"

/**
 * 
 */
UCLASS()
class TEAM02_API UBTTask_SwordAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SwordAttack();

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	
};
