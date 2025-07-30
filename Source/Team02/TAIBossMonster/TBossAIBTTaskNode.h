#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TBossAIBTTaskNode.generated.h"


UCLASS()
class TEAM02_API UTBossAIBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()

public:
		UTBossAIBTTaskNode();

	private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
