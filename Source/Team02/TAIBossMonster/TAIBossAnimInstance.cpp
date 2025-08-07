#include "Team02/TAIBossMonster/TAIBossAnimInstance.h"
#include "TAIBossMonster.h"

UTAIBossAnimInstance::UTAIBossAnimInstance()
	:bIsAttacking(false)
{
	
}
void UTAIBossAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	//애니메이션 이 적용되고 있는 폰 이 보스몹이 맞는지 캐스팅
	BossMonster = Cast<ATAIBossMonster>(TryGetPawnOwner());
	if (BossMonster)
	{	
		MovementComponent = BossMonster->GetCharacterMovement();

		// 몽타주 종료 콜백 델리게이트 바인딩
		if (AttackMontage)
		{
			FOnMontageEnded EndDelegate = FOnMontageEnded::CreateUObject(this, &UTAIBossAnimInstance::OnAttackMontageEnded);
			Montage_SetEndDelegate(EndDelegate, AttackMontage);
		}
	}
}




void UTAIBossAnimInstance::StartAttack()
{	// 상태가 이미 공격중 이거나 몽타주가 없으면 리턴
	if (bIsAttacking || !AttackMontage)
		return;
	//상태 변화
	bIsAttacking = true;
	Montage_Play(AttackMontage);
}
void UTAIBossAnimInstance::EndAttack()
{
	if (!bIsAttacking)
		return;
	//공격 상태 플래그를 꺼서 다음 공격 시작을 허용
	bIsAttacking = false;
	//현재 재생 중인 AttackMontage를 0.2초 동안 부드럽게 멈춤
	Montage_Stop(0.2f, AttackMontage);
}
bool UTAIBossAnimInstance::IsAttacking() const
{
	//상태 리턴
	return bIsAttacking;
}

void UTAIBossAnimInstance::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{	
	if (Montage == AttackMontage)
	{
		bIsAttacking = false;
		
	}
}