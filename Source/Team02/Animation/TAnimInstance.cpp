#include "Animation/TAnimInstance.h"
#include "Character/TCharacterBase.h"
#include "Character/TPlayerCharacter.h"
#include "Character/TNonPlayerCharacter.h"
#include "Team02/Character/TNonPlayerCharacterSword.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UTAnimInstance::NativeInitializeAnimation()
{
	APawn* OwnerPawn = TryGetPawnOwner();
	if (IsValid(OwnerPawn))
	{
		OwnerCharacter = Cast<ATCharacterBase>(OwnerPawn);
		if (IsValid(OwnerCharacter))
		{
			OwnerCharacterMovement = OwnerCharacter->GetCharacterMovement();
		}
	}
}

void UTAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (IsValid(OwnerCharacter) == true && IsValid(OwnerCharacterMovement) == true)
	{
		Velocity = OwnerCharacterMovement->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
                
		float GroundAcceleration = UKismetMathLibrary::VSizeXY(OwnerCharacterMovement->GetCurrentAcceleration());
		bool bIsAccelerated = FMath::IsNearlyZero(GroundAcceleration) == false;
		bShouldMove = (KINDA_SMALL_NUMBER < GroundSpeed) && (bIsAccelerated == true);

		if (ATNonPlayerCharacter* OwnerNPC = Cast<ATNonPlayerCharacter>(OwnerCharacter))
		{
			bShouldMove = KINDA_SMALL_NUMBER < GroundSpeed;
		}

		if (ATNonPlayerCharacterSword* OwnerNPC = Cast<ATNonPlayerCharacterSword>(OwnerCharacter))
		{
			bShouldMove = KINDA_SMALL_NUMBER < GroundSpeed;
		}
		
		bIsFalling = OwnerCharacterMovement ->IsFalling();

		bIsDead = OwnerCharacter->IsDead();
	}
}

void UTAnimInstance::AnimNotify_CheckHit()
{
	if (OnCheckHit.IsBound() == true)
	{
		OnCheckHit.Broadcast();
	}
}

void UTAnimInstance::AnimNotify_PostDead()
{
	if (OnPostDead.IsBound() == true)
	{
		OnPostDead.Broadcast();
	}
}

