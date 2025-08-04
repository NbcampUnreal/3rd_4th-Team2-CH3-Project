#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/TCharacterBase.h"
#include "TNonPlayerCharacter.generated.h"

class ATGunNPCWeapon;


DECLARE_DELEGATE_TwoParams(FOnAttackMontageEnded, UAnimMontage*, bool bInterruped)

UCLASS()
class TEAM02_API ATNonPlayerCharacter : public ATCharacterBase
{
	GENERATED_BODY()

	friend class UBTTask_Attack;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess))
	TSubclassOf<ATGunNPCWeapon> Rifle;

public:
	ATNonPlayerCharacter();

	virtual void BeginPlay() override;
	
	virtual void BeginAttack();

	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped);

public: 
	
	bool bIsNowAttacking;

	void AttachWeapon(TSubclassOf<ATGunNPCWeapon> Weapon) const;
	
protected:
	FOnAttackMontageEnded OnAttackMontageEndedDelegate;
	
	
	
	
};
