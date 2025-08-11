#pragma once

#include "CoreMinimal.h"
#include "Character/TCharacterBase.h"
#include "TNonPlayerCharacterSword.generated.h"

DECLARE_DELEGATE_TwoParams(FOnSwordAttackMontageEnded, UAnimMontage*, bool)

class UAnimMontage;

/**
 * 
 */
UCLASS()
class TEAM02_API ATNonPlayerCharacterSword : public ATCharacterBase
{
	GENERATED_BODY()

	friend class UBTTask_SwordAttack;

public:
	ATNonPlayerCharacterSword();

	virtual void BeginPlay() override;

protected:
	virtual void BeginAttack() override;

	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped) override;



public: 
	static int32 SwordAttackSwordDebug;
	
	bool bIsNowAttacking;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void HandleOnCheckSwordHit();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackSwordMontage;
	
protected:
	FOnSwordAttackMontageEnded OnSwordAttackMontageEndedDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackSwordRange = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackSwordRadius = 20.f;
};
