#include "Team02/AI/BTDecorator_StartWave.h"
#include "AI/TAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_StartWave::UBTDecorator_StartWave()
{
	NodeName = TEXT("IsInWave");
}

bool UBTDecorator_StartWave::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	checkf(bResult == true, TEXT("Super::CalculateRawConditionValue() function has returned false."));
	
	ATAIController* AIController = Cast<ATAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController."));
	
	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(AIController->GetBlackboardComponent());
	checkf(IsValid(BlackboardComponent) == true, TEXT("Invalid BlackboardComponent."));
	
	bool bIsInWave = BlackboardComponent->GetValueAsBool(AIController->IsInWaveKey);
	
	if (bIsInWave == true)
	{
		return true;
	}

	return false;
}