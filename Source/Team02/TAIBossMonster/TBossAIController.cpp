#include "Team02/TAIBossMonster/TBossAIController.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

const float ATBossAIController::PatrolRadius=500.f;
int32 ATBossAIController::ShowAIDebug=1;
const FName ATBossAIController::StartPatrolPositionKey(TEXT("StartPatrolPosition"));
const FName ATBossAIController::EndPatrolPositionKey(TEXT("EndPatrolPosition"));
const FName ATBossAIController::TargetCharacterKey(TEXT("TargetCharacter"));



ATBossAIController::ATBossAIController()
{
	Blackboard=CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BrainComponent"));
}

void ATBossAIController::BeginPlay()
{
	Super::BeginPlay();

	APawn* ControlledCharacter = GetPawn();
	if (IsValid(ControlledCharacter)==true)
	{
		BeginAI(ControlledCharacter);
	}
}
void ATBossAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndAI();
	Super::EndPlay(EndPlayReason);
}

// void ATBossAIController::BossPatrolTimerElapsed()
// {
// 	ACharacter* ControlledCharacter = GetCharacter();
// 	if (IsValid(ControlledCharacter)==true)
// 	{
// 		UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
// 		if (NavigationSystem)
// 		{
// 			FVector ActorLocation = ControlledCharacter->GetActorLocation();
// 			FNavLocation NextLocation;
// 			if (NavigationSystem->GetRandomPointInNavigableRadius(ActorLocation,PatrolRadius,NextLocation)==true)
// 			{
// 				UAIBlueprintHelperLibrary::SimpleMoveToLocation(
// 					this,
// 					NextLocation.Location
// 					);
// 			}
// 		}
// 	}
// }
void ATBossAIController::BeginAI(APawn* InPawn)
{
	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(Blackboard);
	if (IsValid(BlackboardComponent)==true)
	{
		if (UseBlackboard(BlackboardData,BlackboardComponent)==true)
		{
			bool bRunSucceeded=RunBehaviorTree(BehaviorTree);
			checkf(bRunSucceeded==true,TEXT("fail to run behavior tree"));

			BlackboardComponent->SetValueAsVector(StartPatrolPositionKey,InPawn->GetActorLocation());
			
			if (ShowAIDebug)
			{
				UKismetSystemLibrary::PrintString(this,FString::Printf(TEXT("BeginAI")));
			}
		}
	}
}

void ATBossAIController::EndAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (IsValid(BehaviorTreeComponent)==true)
	{
		BehaviorTreeComponent->StopTree();

		if (ShowAIDebug)
		{
			UKismetSystemLibrary::PrintString(this,TEXT("EndAI"));
		}
	}
}


