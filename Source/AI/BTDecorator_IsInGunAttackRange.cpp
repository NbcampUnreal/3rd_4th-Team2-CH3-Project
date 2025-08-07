#include "AI/BTDecorator_IsInGunAttackRange.h"
#include "AI/TAIController.h"
#include "Character/TNonPlayerCharacter.h"
#include "TAIBossMonster/TAIBossMonster.h"
#include "TAIBossMonster/TBossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

const float UBTDecorator_IsInGunAttackRange::AttackRange(1500.f);

UBTDecorator_IsInGunAttackRange::UBTDecorator_IsInGunAttackRange()
{
	NodeName = TEXT("IsInGunAttackRange");
}

bool UBTDecorator_IsInGunAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	checkf(bResult == true, TEXT("Super::CalculateRawConditionValue() function has returned false."));

	ATAIController* AIController1 = Cast<ATAIController>(OwnerComp.GetAIOwner());
	//checkf(IsValid(AIController1) == true, TEXT("Invalid AIController."))

	ATBossAIController* AIController2 = Cast<ATBossAIController>(OwnerComp.GetAIOwner());
	//checkf(IsValid(AIController2)==true, TEXT("AIController is invalid"));

	
	if (IsValid(AIController1) == true)
	{

		ATNonPlayerCharacter* NPC = Cast<ATNonPlayerCharacter>(AIController1->GetPawn());
		checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));

		//타겟 플레이어 설정
		ATCharacterBase* TargetPlayerCharacter = Cast<ATCharacterBase>(
			OwnerComp.GetBlackboardComponent()->GetValueAsObject(ATAIController::TargetCharacterKey));

		//타겟과 AI의 거리를 계산하고 AttackRange 안에 있으면 true 반환
		if (IsValid(TargetPlayerCharacter) == true && TargetPlayerCharacter->IsPlayerControlled() == true)
		{
			return NPC->GetDistanceTo(TargetPlayerCharacter) <= AttackRange;
		}
	}

	if (IsValid(AIController2) == true)
	{

		ATAIBossMonster* NPC = Cast<ATAIBossMonster>(AIController2->GetPawn());
		checkf(IsValid(NPC)==true, TEXT("NPC is invalid"));

		ATCharacterBase* TargetPlayerCharacter = Cast<ATCharacterBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ATBossAIController::TargetCharacterKey));
		if (IsValid(TargetPlayerCharacter)==true && TargetPlayerCharacter->IsPlayerControlled()==true)
		{
			return NPC->GetDistanceTo(TargetPlayerCharacter) <= (AttackRange - 1200);
		}
	}

	return false;
}

