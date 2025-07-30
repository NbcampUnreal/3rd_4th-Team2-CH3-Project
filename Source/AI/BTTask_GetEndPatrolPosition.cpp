#include "AI/BTTask_GetEndPatrolPosition.h"
#include "AI/TAIController.h"
#include "Character/TNonPlayerCharacter.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetEndPatrolPosition::UBTTask_GetEndPatrolPosition()
{
	//노드 이름
	NodeName = TEXT("GetEndPatrolPosition");
}

EBTNodeResult::Type UBTTask_GetEndPatrolPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	//결과가 filed이면 리턴
	if (EBTNodeResult::Failed == Result)
	{
		return Result;
	}

	ATAIController* AIController = Cast<ATAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController"));

	ATNonPlayerCharacter* NPC = Cast<ATNonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC"));

	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(NPC->GetWorld());
	checkf(IsValid(NavigationSystem) == true, TEXT("Invalid NavigationSystem"));

	//StartPatrolPosition의 키를 컨트롤과 연결한다.
	FVector StartPatrolPosition = OwnerComp.GetBlackboardComponent()->GetValueAsVector(ATAIController::StarPatrolPositionKey);
	FNavLocation EndPatrolLocation;
	if (true == NavigationSystem->GetRandomPointInNavigableRadius(StartPatrolPosition, AIController->PatrolRadius,EndPatrolLocation))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(ATAIController::EndPatrolPositionKey, EndPatrolLocation.Location);
		return Result = EBTNodeResult::Succeeded;
	}

	return Result;
}