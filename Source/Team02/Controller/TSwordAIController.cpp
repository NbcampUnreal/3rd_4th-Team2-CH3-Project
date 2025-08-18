#include "Team02/Controller/TSwordAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Area/TCapturePoint.h"
#include "TGameMode.h"

int32 ATSwordAIController::ShowSwordAIDebug(0);

FAutoConsoleVariableRef CVarShowSwordAIDebug(
	TEXT("TProjectSword.ShowAIDebug"),
	ATSwordAIController::ShowSwordAIDebug,
	TEXT(""),
	ECVF_Cheat
);

const FName ATSwordAIController::SwordNPCStartPatrolLocationKey(TEXT("SwordNPCStartPatrolLocation"));
const FName ATSwordAIController::SwordNPCEndPatrolLocationKey(TEXT("SwordNPCEndPatrolLocation"));
const FName ATSwordAIController::SwordNPCTargetCharacterKey(TEXT("TargetCharacter"));
const FName ATSwordAIController::SwordIsInWaveKey(TEXT("IsInWave"));
const FName ATSwordAIController::SwordCapturePointKey(TEXT("CapturePoint"));
const FName ATSwordAIController::SwordBossCapturePointKey(TEXT("BossCapturePoint"));

ATSwordAIController::ATSwordAIController()
{
	SwordNPCPatrolRadius = 800.f;
	
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("SwordNPCBrainComponent"));
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
	
	//게임 모드 배열에 객체의 컨트롤러 등록
	ATGameMode* GameMode = Cast<ATGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (IsValid(GameMode) == true)
	{
		GameMode->RegisterAISwordController(this);
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

			//경계 시작위치를 AI 액터의 현제 위치로 지정
			BlackboardComponent->SetValueAsVector(SwordNPCStartPatrolLocationKey, InPawn->GetActorLocation());

			ATGameMode* GameMode = Cast<ATGameMode>(GetWorld()->GetAuthGameMode());
			if (IsValid(GameMode) == true)
			{
				BlackboardComponent->SetValueAsBool(SwordIsInWaveKey, GameMode->bIsWaveActive);
			

				TArray<AActor*> FoundCapturePoints;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATCapturePoint::StaticClass(), FoundCapturePoints);

				//웨이브에 따라 점령지 위치 바꾸도록 설정
				if (FoundCapturePoints.Num() > 0)
				{
					if (GameMode->WaveIndex == 0)
					{
						AActor* TargetCapturePoint = FoundCapturePoints[0];
						BlackboardComponent->SetValueAsVector(SwordCapturePointKey, TargetCapturePoint->GetActorLocation());
					}
					else if (GameMode->WaveIndex == 1)
					{
						AActor* TargetCapturePoint = FoundCapturePoints[1];
						BlackboardComponent->SetValueAsVector(SwordCapturePointKey, TargetCapturePoint->GetActorLocation());
					}
				}
			}

			//디버깅용
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

		//게임모드의 배열에서 자기 빼기
		ATGameMode* GameMode = Cast<ATGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (IsValid(GameMode) == true)
		{
			GameMode->UnregisterAISwordController(this);
		}

		if (ShowSwordAIDebug == 1)
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("EndAI()")));
		}
	}
}