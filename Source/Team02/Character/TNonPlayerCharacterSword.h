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

private:
	UPROPERTY(EditAnywhere, Category = "Sword", meta = (AllowPrivateAccess))
	TSubclassOf<AActor> Sword;
	
public:
	ATNonPlayerCharacterSword();

	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void AttachWeapon();
	
	void HandleOnCheckSwordHit();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackSwordMontage;

	static int32 SwordAttackSwordDebug;
	
	bool bIsNowAttacking;

protected:
	virtual void BeginAttack() override;

	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped) override;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<AActor> CurrentSword;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> HurtSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundAttenuation> HurtSoundAttenuation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackSwordRange = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackSwordRadius = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSwordNPCIsDead;

	FOnSwordAttackMontageEnded OnSwordAttackMontageEndedDelegate;
};
