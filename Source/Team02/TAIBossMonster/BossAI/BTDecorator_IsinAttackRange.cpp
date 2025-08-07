#include "BTDecorator_IsinAttackRange.h"
#include "TAIBossMonster/TAIBossMonster.h"
#include "TAIBossMonster/TBossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"


 

UBTDecorator_IsinAttackRange::UBTDecorator_IsinAttackRange()
{
	NodeName=TEXT("IsInAttackRange");
	
}

bool UBTDecorator_IsinAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	checkf(bResult==true, TEXT("Super::CalculateRawConditionValue() function has returned false"));

	ATBossAIController* AIController = Cast<ATBossAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController)==true, TEXT("AIController is invalid"));
	
	ATAIBossMonster* NPC = Cast<ATAIBossMonster>(AIController->GetPawn());
	checkf(IsValid(NPC)==true, TEXT("NPC is invalid"));

	ATCharacterBase* TargetPlayerCharacter = Cast<ATCharacterBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ATBossAIController::TargetCharacterKey));
	if (IsValid(TargetPlayerCharacter)==true && TargetPlayerCharacter->IsPlayerControlled()==true)
	{
		return NPC->GetDistanceTo(TargetPlayerCharacter) <=AttackRange;
	}
	
	return false;
}

