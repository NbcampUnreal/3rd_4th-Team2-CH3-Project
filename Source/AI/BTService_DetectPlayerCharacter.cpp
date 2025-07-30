#include "AI/BTService_DetectPlayerCharacter.h"
#include "AI/TAIController.h"
#include "Character/TCharacterBase.h"
#include "Character/TNonPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"

UBTService_DetectPlayerCharacter::UBTService_DetectPlayerCharacter(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("DetectPlayerCharacter");
	//시간 간격
	Interval = 1.f;
}

void UBTService_DetectPlayerCharacter::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ATAIController* AIC = Cast<ATAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIC) == true)
	{
		ATNonPlayerCharacter* NPC = Cast<ATNonPlayerCharacter>(AIC->GetPawn());
		if (IsValid(NPC) == true)
		{
			UWorld* World = NPC->GetWorld();
			if (IsValid(World) == true)
			{
				//중심점
				FVector CenterPosition = NPC->GetActorLocation();
				float DetectRadius = 300.f;
				TArray<FOverlapResult> OverlapResults;
				FCollisionQueryParams CollisionQueryParams(NAME_None, false, NPC);
				// 시야
				bool bResult = World->OverlapMultiByChannel(
					OverlapResults,
					CenterPosition,
					FQuat::Identity,
					ECollisionChannel::ECC_GameTraceChannel12,
					FCollisionShape::MakeSphere(DetectRadius),
					CollisionQueryParams
					);

				//시야에 오버랩
				if (bResult == true)
				{
					for (auto const& OverlapResult : OverlapResults)
					{
						ATCharacterBase* PC = Cast<ATCharacterBase>(OverlapResult.GetActor());
						
						if (IsValid(PC) == true && PC->GetController()->IsPlayerController() == true)
						{
							//시야에 플레이어가 오버랩되면
							OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATAIController::TargetCharacterKey, PC);

							//디버깅용
							if (ATAIController::ShowAIDebug == 1)
							{
								UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Detected!")));
								DrawDebugSphere(
									World,
									CenterPosition,
									DetectRadius,
									16,
									FColor::Red,
									false,
									0.5f
									);

								DrawDebugPoint(
									World,
									PC->GetActorLocation(),
									10.f,
									FColor::Red,
									false,
									0.5f
									);

								DrawDebugLine(
									World,
									NPC->GetActorLocation(),
									PC->GetActorLocation(),
									FColor::Blue,
									false,
									0.5f,
									0u,
									3.f
									);
							}

							break;
						}
						else
						{
							//Player가 아니라면
							OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATAIController::TargetCharacterKey, nullptr);

							if (ATAIController::ShowAIDebug == 1)
							{
								DrawDebugSphere(
									World,
									CenterPosition,
									DetectRadius,
									16,
									FColor::Green,
									false,
									0.5f
									);
							}
						}
					}
				}
				else
				{
					//시야에 오버랩 되지 않으면
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATAIController::TargetCharacterKey, nullptr);
				}

				if (ATAIController::ShowAIDebug == 1)
				{
					DrawDebugSphere(
						World,
						CenterPosition,
						DetectRadius,
						16,
						FColor::Green,
						false,
						0.5f);
				}
			}
		}
	}
}