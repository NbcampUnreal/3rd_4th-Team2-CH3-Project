#include "Team02/TAIBossMonster/TAIBossAnimInstance.h"
#include "TAIBossMonster.h"

UTAIBossAnimInstance::UTAIBossAnimInstance()
	:bIsAttacking(false)
{
	
}
void UTAIBossAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	BossMonster = Cast<ATAIBossMonster>(TryGetPawnOwner());
	if (BossMonster)
	{
		MovementComponent = BossMonster->GetCharacterMovement();

		// 몽타주 종료 콜백 델리게이트 바인딩
		if (AttackMontage)
		{
			
		}
	}
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