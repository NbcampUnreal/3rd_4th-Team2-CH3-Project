#include "TAIBossBTService.h"
#include "TAIBossMonster.h"
#include "TBossAIController.h"
#include "Character/TPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"

UTAIBossBTService::UTAIBossBTService(const FObjectInitializer& ObjectInitializer)
	
{
	
	NodeName = TEXT("DetecPlayerCharacter");
	Interval=1.f;
}

void UTAIBossBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	//
	ATBossAIController* AIC = Cast<ATBossAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIC)==true)
	{ 
		ATAIBossMonster* NPC = Cast<ATAIBossMonster>(AIC->GetPawn());
	
		if (IsValid(NPC)==true)
		{
			UWorld* World = NPC->GetWorld();
			
			if (IsValid(World)==true)
			{	//레디어스
				
				FVector CenterPosition = NPC->GetActorLocation();
				float DetectRadius = 500.f;
				TArray<FOverlapResult> OverlapResults;
				FCollisionQueryParams CollisionQueryParams(NAME_None,false,NPC);
				//스피어콜리젼 생성
				bool bResult = World-> OverlapMultiByChannel(
					OverlapResults,
					CenterPosition,
					FQuat::Identity,
					ECollisionChannel::ECC_GameTraceChannel12,
					FCollisionShape::MakeSphere(DetectRadius),
					CollisionQueryParams
				);
				
				UBlackboardComponent* BlackboardComp=OwnerComp.GetBlackboardComponent();
				BlackboardComp->SetValueAsObject(ATBossAIController::TargetCharacterKey,nullptr);
				
				// 오버랩 되는 액터가 있으면 케릭터만 인식할수 있게 캐스팅
				if (bResult)
				{
					for (auto const& OverlapResult : OverlapResults)
					{
						ATCharacterBase* PC = Cast<ATCharacterBase>(OverlapResult.GetActor());
						if (IsValid(PC)==true && PC->GetController()->IsPlayerController()==true)
						{
							OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATBossAIController::TargetCharacterKey,PC);
						/////////디버깅
						 	if (ATBossAIController::ShowAIDebug == 1)
						 	{
						 		
						// 		UKismetSystemLibrary::PrintString(this,FString::Printf(TEXT("Detected!")));
						// 		DrawDebugSphere(World, CenterPosition, DetectRadius, 16, FColor::Red, false, 3.0f);
						// 		DrawDebugPoint(World, PC->GetActorLocation(), 10.f, FColor::Red, false, 1.0f);
						// 		DrawDebugLine(World, NPC->GetActorLocation(), PC->GetActorLocation(), FColor::Red, false, 1.0f, 0u, 3.f);
						 	}
						 	break;
						 }
						//  else
						//  {
						//  Player 아니면 nullptr	
						// 		OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATBossAIController::TargetCharacterKey,nullptr);
						//
						// 		 if (ATBossAIController::ShowAIDebug == 1)
						// 		 {
						// 		 	DrawDebugSphere(World, CenterPosition, DetectRadius, 16, FColor::Green, false, 5.f);
						// 		 }
						// 	
						// }
					}
					
				}
				// else
				// {   //오버랩 되지 않으면
				// 	OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATBossAIController::TargetCharacterKey,nullptr);
				// }
				// // if (ATBossAIController::ShowAIDebug == 1)
				// {
				// 	DrawDebugSphere(World, CenterPosition, DetectRadius, 16, FColor::Green, false, 0.5f);
				// }
			}
		}
	}
}


