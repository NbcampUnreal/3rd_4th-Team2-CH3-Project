#include "AI/BTTask_GunAttack.h"
#include "AI/TAIController.h"
#include "Character/TNonPlayerCharacter.h"

UBTTask_GunAttack::UBTTask_GunAttack()
{
	bNotifyTick = true;
}

void UBTTask_GunAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	ATAIController* AIController = Cast<ATAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController."));

	ATNonPlayerCharacter* NPC = Cast<ATNonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));

	if (NPC->bIsNowAttacking == false)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_GunAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ATAIController* AIController = Cast<ATAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController."));

	ATNonPlayerCharacter* NPC = Cast<ATNonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));

	NPC->BeginAttack();

	return EBTNodeResult::InProgress;
}