#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TAIBossAnimInstance.generated.h"

UCLASS()
class TEAM02_API UTAIBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UTAIBossAnimInstance();
	
protected:
	// 공격 시작/종료는 내부, 상속, 블루프린트 전용으로 관리
	UFUNCTION(BlueprintCallable, Category = "Boss")
	virtual void StartAttack();
	UFUNCTION(BlueprintCallable, Category = "Boss")
	virtual void EndAttack();
	UFUNCTION(BlueprintCallable, Category = "Boss")
	bool IsAttacking() const;
	
	virtual void NativeInitializeAnimation() override;
	// 몽타주 종료 콜백 바인딩
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	// Transient 저장 복원 충돌방지
	UPROPERTY(BlueprintReadOnly,Transient)
	TObjectPtr<class ATAIBossMonster> BossMonster;
	UPROPERTY(BlueprintReadOnly,Transient)
	TObjectPtr<class UCharacterMovementComponent> MovementComponent;
	
	
protected:
	//패트롤
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Movement")
	bool bIsPatrolling;
	//상태 반환
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	bool bIsAttacking;
	//공격 몽타주 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Montage")
	UAnimMontage* AttackMontage;
	


	
	
	

	

	
	
};
