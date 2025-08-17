#include "Team02/AI/BTService_IsInWave_DetectPlayer.h"
#include "AI/TAIController.h"
#include "Controller/TSwordAIController.h"
#include "Character/TCharacterBase.h"
#include "Character/TNonPlayerCharacter.h"
#include "Character/TNonPlayerCharacterSword.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"

UBTService_IsInWave_DetectPlayer::UBTService_IsInWave_DetectPlayer(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("IsInWaveDetectPlayerCharacter");
	//시간 간격
	Interval = 0.3f;
}

void UBTService_IsInWave_DetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ATAIController* GunAIC = Cast<ATAIController>(OwnerComp.GetAIOwner());
	ATSwordAIController* SwordAIC = Cast<ATSwordAIController>(OwnerComp.GetAIOwner());
	
	if (IsValid(GunAIC) == true)
	{
		ATNonPlayerCharacter* NPC = Cast<ATNonPlayerCharacter>(GunAIC->GetPawn());
		if (IsValid(NPC) == true)
		{
			UWorld* World = NPC->GetWorld();
			if (IsValid(World) == true)
			{
				//중심점
				FVector CenterPosition = NPC->GetActorLocation();
				float DetectRadius = 500000.0f;
				
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
							//장애물 시야
							FHitResult HitResult;

							FCollisionQueryParams LineTraceQueryParams;
							LineTraceQueryParams.AddIgnoredActor(NPC);
							LineTraceQueryParams.AddIgnoredActor(PC);

							bool bHit = World->LineTraceSingleByChannel(
								HitResult,
								CenterPosition,
								PC->GetActorLocation(),
								ECollisionChannel::ECC_Visibility,
								LineTraceQueryParams
								);

							if (!bHit)
							{
								//시야에 플레이어가 오버랩되면
								OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATAIController::TargetCharacterKey, PC);
							}
							
							
							//디버깅용
							if (ATAIController::ShowAIDebug == 1)
							{

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
					0.5f
					);
				}
			}
		}
	}

	if (IsValid(SwordAIC) == true)
	{
		ATNonPlayerCharacterSword* NPC = Cast<ATNonPlayerCharacterSword>(SwordAIC->GetPawn());
		if (IsValid(NPC) == true)
		{
			UWorld* World = NPC->GetWorld();
			if (IsValid(World) == true)
			{
				//중심점
				FVector CenterPosition = NPC->GetActorLocation();
				float DetectRadius = 800000.0f;
				
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
							//장애물 시야
							FHitResult HitResult;

							FCollisionQueryParams LineTraceQueryParams;
							LineTraceQueryParams.AddIgnoredActor(NPC);
							LineTraceQueryParams.AddIgnoredActor(PC);

							bool bHit = World->LineTraceSingleByChannel(
							HitResult,
							CenterPosition,
							PC->GetActorLocation(),
							ECollisionChannel::ECC_Visibility,
							LineTraceQueryParams
							);

							if (!bHit)
							{
								//시야에 플레이어가 오버랩되면
								OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATSwordAIController::SwordNPCTargetCharacterKey, PC);
							}
							
							
							//디버깅용
							if (ATSwordAIController::ShowSwordAIDebug == 1)
							{

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
							OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATSwordAIController::SwordNPCTargetCharacterKey, nullptr);

							if (ATSwordAIController::ShowSwordAIDebug == 1)
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
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATSwordAIController::SwordNPCTargetCharacterKey, nullptr);
				}

				if (ATSwordAIController::ShowSwordAIDebug == 1)
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
}