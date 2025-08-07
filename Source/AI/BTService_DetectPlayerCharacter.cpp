#include "AI/BTService_DetectPlayerCharacter.h"
#include "AI/TAIController.h"
#include "Controller/TSwordAIController.h"
#include "Character/TCharacterBase.h"
#include "Character/TNonPlayerCharacter.h"
#include "Character/TNonPlayerCharacterSword.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"

UBTService_DetectPlayerCharacter::UBTService_DetectPlayerCharacter(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("DetectPlayerCharacter");
	//시간 간격
	Interval = 0.5f;
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
				float DetectRadius = 1300.0f;
				float VisionAngle = 80.0f;
				
				//시야 시작 위치
				const FVector ConeOrigin = NPC->GetMesh()->GetSocketLocation("head");
				const FVector ForwardVector = NPC->GetActorForwardVector();
				
				TArray<FOverlapResult> OverlapResults;
				FCollisionQueryParams CollisionQueryParams(NAME_None, false, NPC);

				// 시야
				bool bResult = World->OverlapMultiByChannel(
					OverlapResults,
					ConeOrigin,
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
							
							FVector DirectionToTarget = (PC->GetActorLocation() - ConeOrigin).GetSafeNormal();
							//내적 구하기
							float DotProduct = FVector::DotProduct(ForwardVector, DirectionToTarget);
							//시야각의 절반에 해당하는 코사인 값 계산
							float AngleThreshold = FMath::Cos(FMath::DegreesToRadians(VisionAngle / 2.0f));

							if (DotProduct >= AngleThreshold)
							{
								//장애물 시야
								FHitResult HitResult;

								FCollisionQueryParams LineTraceQueryParams;
								LineTraceQueryParams.AddIgnoredActor(NPC);
								LineTraceQueryParams.AddIgnoredActor(PC);

								bool bHit = World->LineTraceSingleByChannel(
								HitResult,
								ConeOrigin,
								PC->GetActorLocation(),
								ECollisionChannel::ECC_Visibility,
								LineTraceQueryParams
								);

								if (!bHit)
								{
									//시야에 플레이어가 오버랩되면
									OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATAIController::TargetCharacterKey, PC);
								}
								
							}
							
							//디버깅용
							if (ATAIController::ShowAIDebug == 1)
							{

								DrawDebugCone(
								World,
								ConeOrigin,
								ForwardVector,
								DetectRadius,
								FMath::DegreesToRadians(VisionAngle / 2.0f),
								FMath::DegreesToRadians(VisionAngle / 2.0f),
								12,
								FColor::Red,
								false,
								0.5f,
								3.0f
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
								
								DrawDebugCone(
								World,
								ConeOrigin,
								ForwardVector,
								DetectRadius,
								FMath::DegreesToRadians(VisionAngle / 2.0f),
								FMath::DegreesToRadians(VisionAngle / 2.0f),
								12,
								FColor::Green,
								false,
								0.5f,
								3.0f
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

					DrawDebugCone(
					World,
					ConeOrigin,
					ForwardVector,
					DetectRadius,
					FMath::DegreesToRadians(VisionAngle / 2.0f),
					FMath::DegreesToRadians(VisionAngle / 2.0f),
					12,
					FColor::Green,
					false,
					0.5f,
					3.0f
					);
				}
			}
		}
	}


	ATSwordAIController* SwordAIC = Cast<ATSwordAIController>(OwnerComp.GetAIOwner());
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
				float DetectRadius = 1300.0f;
				float VisionAngle = 80.0f;
				
				//시야 시작 위치
				const FVector ConeOrigin = NPC->GetMesh()->GetSocketLocation("head");
				const FVector ForwardVector = NPC->GetActorForwardVector();
				
				TArray<FOverlapResult> OverlapResults;
				FCollisionQueryParams CollisionQueryParams(NAME_None, false, NPC);

				// 시야
				bool bResult = World->OverlapMultiByChannel(
					OverlapResults,
					ConeOrigin,
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
							
							FVector DirectionToTarget = (PC->GetActorLocation() - ConeOrigin).GetSafeNormal();
							//내적 구하기
							float DotProduct = FVector::DotProduct(ForwardVector, DirectionToTarget);
							//시야각의 절반에 해당하는 코사인 값 계산
							float AngleThreshold = FMath::Cos(FMath::DegreesToRadians(VisionAngle / 2.0f));

							if (DotProduct >= AngleThreshold)
							{
								//장애물 시야
								FHitResult HitResult;

								FCollisionQueryParams LineTraceQueryParams;
								LineTraceQueryParams.AddIgnoredActor(NPC);
								LineTraceQueryParams.AddIgnoredActor(PC);

								bool bHit = World->LineTraceSingleByChannel(
								HitResult,
								ConeOrigin,
								PC->GetActorLocation(),
								ECollisionChannel::ECC_Visibility,
								LineTraceQueryParams
								);

								if (!bHit)
								{
									//시야에 플레이어가 오버랩되면
									OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATAIController::TargetCharacterKey, PC);
								}
								
							}
							
							//디버깅용
							if (ATAIController::ShowAIDebug == 1)
							{

								DrawDebugCone(
								World,
								ConeOrigin,
								ForwardVector,
								DetectRadius,
								FMath::DegreesToRadians(VisionAngle / 2.0f),
								FMath::DegreesToRadians(VisionAngle / 2.0f),
								12,
								FColor::Red,
								false,
								0.5f,
								3.0f
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
								
								DrawDebugCone(
								World,
								ConeOrigin,
								ForwardVector,
								DetectRadius,
								FMath::DegreesToRadians(VisionAngle / 2.0f),
								FMath::DegreesToRadians(VisionAngle / 2.0f),
								12,
								FColor::Green,
								false,
								0.5f,
								3.0f
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

					DrawDebugCone(
					World,
					ConeOrigin,
					ForwardVector,
					DetectRadius,
					FMath::DegreesToRadians(VisionAngle / 2.0f),
					FMath::DegreesToRadians(VisionAngle / 2.0f),
					12,
					FColor::Green,
					false,
					0.5f,
					3.0f
					);
				}
			}
		}
	}
}