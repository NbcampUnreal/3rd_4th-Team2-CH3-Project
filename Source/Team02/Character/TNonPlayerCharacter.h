#pragma once

#include "CoreMinimal.h"
#include "Character/TCharacterBase.h"
#include "TNonPlayerCharacter.generated.h"

class ATGunNPCWeapon;

DECLARE_DELEGATE_TwoParams(FOnAttackMontageEnded, UAnimMontage*, bool bInterrupted)

UCLASS()
class TEAM02_API ATNonPlayerCharacter : public ATCharacterBase
{
	GENERATED_BODY()

	friend class UBTTask_GunAttack;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess))
	TSubclassOf<ATGunNPCWeapon> Rifle;

public:
	ATNonPlayerCharacter();

	virtual void BeginPlay() override;

	void AttachWeapon(TSubclassOf<ATGunNPCWeapon> Weapon);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	void HandleOnCheckHit();

	static int32 ShowGunAttackDebug;

	bool bIsNowAttacking;
	

protected:
	virtual void BeginAttack();

	virtual void EndAttack(UAnimMontage* InMontage, bool bIbterruped);

	virtual void HandleOnPostCharacterDead() override;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<ATGunNPCWeapon> CurrentRifle;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> HurtSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundAttenuation> HurtSoundAttenuation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dead")
	bool SwordNPCIsDead;
	
	FOnAttackMontageEnded OnAttackMontageEndedDelegate;
	
};
