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

protected:
	virtual void BeginAttack();

	virtual void EndAttack(UAnimMontage* InMontage, bool bIbterruped);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<ATGunNPCWeapon> CurrentRifle;

public: 
	
	bool bIsNowAttacking;

	void AttachWeapon(TSubclassOf<ATGunNPCWeapon> Weapon);

	static int32 ShowGunAttackDebug;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	virtual void HandleOnCheckHit();
	
protected:
	FOnAttackMontageEnded OnAttackMontageEndedDelegate;
	
};
