#include "Team02/AI/BTTask_SwordAttack.h"
#include "Controller/TSwordAIController.h"
#include "Character/TNonPlayerCharacterSword.h"

UBTTask_SwordAttack::UBTTask_SwordAttack()
{
	bNotifyTick = true;
}

void UBTTask_SwordAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	ATSwordAIController* AIController = Cast<ATSwordAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController."));

	ATNonPlayerCharacterSword* NPC = Cast<ATNonPlayerCharacterSword>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));

	if (NPC->bIsNowAttacking == false)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_SwordAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ATSwordAIController* AIController = Cast<ATSwordAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController."));

	ATNonPlayerCharacterSword* NPC = Cast<ATNonPlayerCharacterSword>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));

	NPC->BeginAttack();

	return EBTNodeResult::InProgress;
}
