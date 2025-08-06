#pragma once

#include "CoreMinimal.h"
#include "Character/TCharacterBase.h"
#include "TNonPlayerCharacterSword.generated.h"

DECLARE_DELEGATE_TwoParams(FOnSwordAttackMontageEnded, UAnimMontage*, bool)

/**
 * 
 */
UCLASS()
class TEAM02_API ATNonPlayerCharacterSword : public ATCharacterBase
{
	GENERATED_BODY()

/*public:
	ATNonPlayerCharacterSword();

	virtual void BeginPlay() override;

protected:
	virtual void BeginAttack() override;

	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped) override;

public: 
	
	bool bIsNowAttacking;*/
};
