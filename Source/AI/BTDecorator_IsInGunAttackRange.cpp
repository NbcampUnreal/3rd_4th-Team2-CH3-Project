#include "AI/BTDecorator_IsInGunAttackRange.h"
#include "AI/TAIController.h"
#include "Character/TNonPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

//공격범위를 전역변수로 지정
const float UBTDecorator_IsInGunAttackRange::AttackRange(200.f);

UBTDecorator_IsInGunAttackRange::UBTDecorator_IsInGunAttackRange()
{
	NodeName = TEXT("IsInGunAttackRange");
}

bool UBTDecorator_IsInGunAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	checkf(bResult == true, TEXT("Super::CalculateRawConditionValue() function has returned false."));

	ATAIController* AIController = Cast<ATAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController."))

	ATNonPlayerCharacter* NPC = Cast<ATNonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));

	//타겟 플레이어 설정
	ATCharacterBase* TargetPlayerCharacter = Cast<ATCharacterBase>(
		OwnerComp.GetBlackboardComponent()->GetValueAsObject(ATAIController::TargetCharacterKey));

	//타겟과 AI의 거리를 계산하고 AttackRange 안에 있으면 true 반환
	if (IsValid(TargetPlayerCharacter) == true && TargetPlayerCharacter->IsPlayerControlled() == true)
	{
		return NPC->GetDistanceTo(TargetPlayerCharacter) <= AttackRange;
	}

	return false;
}
