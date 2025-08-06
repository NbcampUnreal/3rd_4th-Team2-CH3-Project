#include "AI/TAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Item/TGunNPCWeapon.h"
#include "Kismet/KismetSystemLibrary.h"

//순찰 범위
const float ATAIController::PatrolRadius(500.f);
int32 ATAIController::ShowAIDebug(0);

FAutoConsoleVariableRef CVarShowAIDebug(
	TEXT("TProject.ShowAIDebug"),
	ATAIController::ShowAIDebug,
	TEXT(""),
	ECVF_Cheat
);

//블랙보드 키 연결
const FName ATAIController::StarPatrolPositionKey(TEXT("StartPatrolPosition"));
const FName ATAIController::EndPatrolPositionKey(TEXT("EndPatrolPosition"));
const FName ATAIController::TargetCharacterKey(TEXT("TargetCharacter"));

ATAIController::ATAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("GunNPCBlackboard"));
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("GunNPCBrainComponent"));
	
}

void ATAIController::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATAIController::OnPossess(APawn* InPawn)
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
void ATAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndAI();

	Super::EndPlay(EndPlayReason);
}



void ATAIController::BeginAI(APawn* InPawn)
{
	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(Blackboard);
	if (IsValid(BlackboardComponent) == true)
	{
		if (UseBlackboard(BlackboardDataAsset, BlackboardComponent) == true)
		{
			bool bRunSucceeded = RunBehaviorTree(BehaviorTree);
			checkf(bRunSucceeded == true, TEXT("Fail to run behavior tree."))

			//경계 시작위치를 AI 액터의 현제 위치로 지정
			BlackboardComponent->SetValueAsVector(StarPatrolPositionKey, InPawn->GetActorLocation());

			if (ShowAIDebug == 1)
			{
				UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("BeginAI()")));
			}
		}
	}
}

void ATAIController::EndAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	
	if (IsValid(BehaviorTreeComponent) == true)
	{
		BehaviorTreeComponent->StopTree();

		if (ShowAIDebug == 1)
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("EndAI()")));
		}
	}
}