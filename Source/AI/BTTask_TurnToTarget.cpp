#include "AI/BTTask_TurnToTarget.h"
#include "AI/TAIController.h"
#include "Character/TNonPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("TurnToTargetActor");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	ATAIController* AIController = Cast<ATAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController."));

	ATNonPlayerCharacter* NPC = Cast<ATNonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));

	if (ATCharacterBase* TargetPC = Cast<ATCharacterBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AIController->TargetCharacterKey)))
	{
		FVector LookVector = TargetPC->GetActorLocation() - NPC->GetActorLocation();
		LookVector.Z = 0.f;
		FRotator TargetRotation = FRotationMatrix::MakeFromX(LookVector).Rotator();
		NPC->SetActorRotation(FMath::RInterpTo(NPC->GetActorRotation(),TargetRotation, GetWorld()->GetDeltaSeconds(), 50.f));

		return Result = EBTNodeResult::Succeeded;
	}

	return Result = EBTNodeResult::Failed;
}
