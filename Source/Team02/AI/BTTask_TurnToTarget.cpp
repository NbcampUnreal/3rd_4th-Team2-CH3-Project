#include "AI/BTTask_TurnToTarget.h"
#include "AI/TAIController.h"
#include "Controller/TSwordAIController.h"
#include "Character/TNonPlayerCharacter.h"
#include "Character/TNonPlayerCharacterSword.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("TurnToTargetActor");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	ATAIController* GunAIController = Cast<ATAIController>(OwnerComp.GetAIOwner());
	ATSwordAIController* SwordAIController = Cast<ATSwordAIController>(OwnerComp.GetAIOwner());

	//원거리 NPC용
	if (IsValid(GunAIController) == true)
	{
		ATNonPlayerCharacter* NPC = Cast<ATNonPlayerCharacter>(GunAIController->GetPawn());
		checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));

		if (ATCharacterBase* TargetPC = Cast<ATCharacterBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GunAIController->TargetCharacterKey)))
		{
			FVector LookVector = TargetPC->GetActorLocation() - NPC->GetActorLocation();
			LookVector.Z = 0.f;
			FRotator TargetRotation = FRotationMatrix::MakeFromX(LookVector).Rotator();
			NPC->SetActorRotation(FMath::RInterpTo(NPC->GetActorRotation(),TargetRotation, GetWorld()->GetDeltaSeconds(), 50.f));

			return Result = EBTNodeResult::Succeeded;
		}
	}

	//근접 NPC 용
	if (IsValid(SwordAIController) == true)
	{
		ATNonPlayerCharacterSword* NPC = Cast<ATNonPlayerCharacterSword>(SwordAIController->GetPawn());
		checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));

		if (ATCharacterBase* TargetPC = Cast<ATCharacterBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(SwordAIController->SwordNPCTargetCharacterKey)))
		{
			FVector LookVector = TargetPC->GetActorLocation() - NPC->GetActorLocation();
			LookVector.Z = 0.f;
			FRotator TargetRotation = FRotationMatrix::MakeFromX(LookVector).Rotator();
			NPC->SetActorRotation(FMath::RInterpTo(NPC->GetActorRotation(),TargetRotation, GetWorld()->GetDeltaSeconds(), 80.f));

			return Result = EBTNodeResult::Succeeded;
		}
	}
	
	return Result = EBTNodeResult::Failed;
}
