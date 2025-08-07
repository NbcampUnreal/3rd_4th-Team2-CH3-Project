#include "Team02/Controller/TSwordAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

int32 ATSwordAIController::ShowSwordAIDebug(0);

FAutoConsoleVariableRef CVarShowSwordAIDebug(
	TEXT("TProjectSword.ShowAIDebug"),
	ATSwordAIController::ShowSwordAIDebug,
	TEXT(""),
	ECVF_Cheat
);

ATSwordAIController::ATSwordAIController()
{
	SwordNPCPatrolRadius = 800.f;
	
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("SwordNPCBlackboard"));
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("SwordNPCBrainComponent"));
	
}

void ATSwordAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ATSwordAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// 설정한 폰이 Ai의 컨트롤 폰이 된다
	APawn* ControlledPawn = GetPawn();
	if (IsValid(ControlledPawn) == true)
	{
		BeginAI(ControlledPawn);
	}
}

//플레이를 종료할때 출력되는 함수
void ATSwordAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndAI();

	Super::EndPlay(EndPlayReason);
}


void ATSwordAIController::BeginAI(APawn* InPawn)
{
	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(Blackboard);
	if (IsValid(BlackboardComponent) == true)
	{
		if (UseBlackboard(SwordNPCBlackboardDataAsset, BlackboardComponent) == true)
		{
			bool bRunSucceeded = RunBehaviorTree(SwordNPCBehaviorTree);
			checkf(bRunSucceeded == true, TEXT("Fail to run behavior tree."))

			/*//경계 시작위치를 AI 액터의 현제 위치로 지정
			BlackboardComponent->SetValueAsVector(StarPatrolPositionKey, InPawn->GetActorLocation());*/

			if (ShowSwordAIDebug == 1)
			{
				UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("BeginAI()")));
			}
		}
	}
}

void ATSwordAIController::EndAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	
	if (IsValid(BehaviorTreeComponent) == true)
	{
		BehaviorTreeComponent->StopTree();

		if (ShowSwordAIDebug == 1)
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("EndAI()")));
		}
	}
}