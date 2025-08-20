#include "TBossAIBTTaskNode.h"
#include "NavigationSystem.h"
#include "TAIBossMonster/TAIBossMonster.h"
#include "TAIBossMonster/TBossAIController.h"
#include "AI/TAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UTBossAIBTTaskNode::UTBossAIBTTaskNode()
{
	NodeName = TEXT("GetEndPatrolPosition");  
}

EBTNodeResult::Type UTBossAIBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);


	if (EBTNodeResult::Failed==Result)
	{
		return Result;
	}
	ATBossAIController* AIController = Cast<ATBossAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController)==true,TEXT("Invalid NPC"));
	ATAIBossMonster* NPC = Cast<ATAIBossMonster>(AIController->GetCharacter());
	checkf(IsValid(NPC)==true,TEXT("Invalid NPC"));
	UNavigationSystemV1* NavigationSystem=UNavigationSystemV1::GetNavigationSystem(NPC->GetWorld());
	checkf(IsValid(NavigationSystem)==true,TEXT("Invalid NavigationSystem"));

	FVector StartPosition=OwnerComp.GetBlackboardComponent()->GetValueAsVector(ATBossAIController::StartPatrolPositionKey);
	FNavLocation EndPtrolLocation;
	if (true==NavigationSystem->GetRandomPointInNavigableRadius(StartPosition,AIController->PatrolRadius,EndPtrolLocation))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(ATBossAIController::EndPatrolPositionKey,EndPtrolLocation.Location);
		return Result=EBTNodeResult::Succeeded;
	}
return Result;
}