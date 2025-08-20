#include "Team02/TAIBossMonster/TAIBossMonster.h"
#include "Animation/TAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

ATAIBossMonster::ATAIBossMonster()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ATAIBossMonster::BeginPlay()
{
	Super::BeginPlay();

	if (false==IsPlayerControlled())
	{
		bUseControllerRotationYaw = false;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);

		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		
	}
}

 void ATAIBossMonster::BeginAttack()
 {
// 	UTAnimInstance* AnimInstance = Cast<UTAnimInstance>(GetMesh()->GetAnimInstance());
// 	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance"));
//
// 	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
// 	if (IsValid(AnimInstance) == true && IsValid(AttackMelee) == true && AnimInstance->Montage_IsPlaying(AttackMeleeMontage) == false)
// 	{
//
// 	}
}

void ATAIBossMonster::EndAttack(class UAnimMontage* InMontage,bool bInterruped)
{
	
}