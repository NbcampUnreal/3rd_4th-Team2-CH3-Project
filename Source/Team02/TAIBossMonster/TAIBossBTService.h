#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "TAIBossBTService.generated.h"



UCLASS()
class TEAM02_API UTAIBossBTService : public UBTService
{
	GENERATED_BODY()

public:
	UTAIBossBTService(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
};
