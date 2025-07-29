#include "AI/TAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

//순찰 주기
const float ATAIController::PatrolRepeatInterval(3.f);
//순찰 범위
const float ATAIController::PatrolRadius(500.f);

ATAIController::ATAIController()
{
}

void ATAIController::BeginPlay()
{
	Super::BeginPlay();

	//순찰 타이머
	GetWorld()->GetTimerManager().SetTimer(
		PatrolTimerHandle,
		this,
		&ThisClass::OnPatrolTimerElapsed,
		PatrolRepeatInterval,
		true
	);
		
}

//플레이를 종료할때 출력되는 함수
void ATAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//경계 타이머를 끝낸다
	GetWorld()->GetTimerManager().ClearTimer(PatrolTimerHandle);

	Super::EndPlay(EndPlayReason);
}

//랜덤으로 경계하며 돌아다니는 함수
void ATAIController::OnPatrolTimerElapsed()
{
	APawn* ControlledPawn = GetPawn();
	
	if (IsValid(ControlledPawn) == true)
	{
		//월드의 네비게이션 시스템을 가져온다.
		UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());

		if (IsValid(NavigationSystem) == true)
		{
			//현제 폰의 위치
			FVector ActorLocation = ControlledPawn->GetActorLocation();
			//랜덤 위지
			FNavLocation NextLocation;

			//경계 원안에서 랜덤 위치 찾기
			if (NavigationSystem->GetRandomPointInNavigableRadius(
				ActorLocation,
				PatrolRadius,
				NextLocation) == true)
			{
				//찾은 랜덤 위치로 이동
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
			}
		}
	}
}