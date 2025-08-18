#include "Team02/AI/BTDecorator_StartWave.h"
#include "AI/TAIController.h"
#include "Controller/TSwordAIController.h"
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

	//원거리 NPC 용
	if (IsValid(AIController) == true)
	{
		UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(AIController->GetBlackboardComponent());
		checkf(IsValid(BlackboardComponent) == true, TEXT("Invalid BlackboardComponent."));
	
		bool bIsInWave = BlackboardComponent->GetValueAsBool(AIController->IsInWaveKey);
	
		if (bIsInWave == true)
		{
			return true;
		}
	}

	ATSwordAIController* AISwordController = Cast<ATSwordAIController>(OwnerComp.GetAIOwner());

	//근접 NPC용
	if (IsValid(AISwordController) == true)
	{
		UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(AISwordController->GetBlackboardComponent());
		checkf(IsValid(BlackboardComponent) == true, TEXT("Invalid BlackboardComponent."));
	
		bool bIsInWave = BlackboardComponent->GetValueAsBool(AISwordController->SwordIsInWaveKey);
	
		if (bIsInWave == true)
		{
			return true;
		}
	}

	return false;
}