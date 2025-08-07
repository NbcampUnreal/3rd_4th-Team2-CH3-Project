#include "Team02/TAIBossMonster/TAIBossAnimInstance.h"

UTAIBossAnimInstance::UTAIBossAnimInstance()
	:bIsAttacking(false)
{
	
}
void UTAIBossAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
}

void UTAIBossAnimInstance::StartAttack()
{
	
}
void UTAIBossAnimInstance::EndAttack()
{
	
}
bool UTAIBossAnimInstance::IsAttacking() const
{
	return bIsAttacking;
}

void UTAIBossAnimInstance::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	
}