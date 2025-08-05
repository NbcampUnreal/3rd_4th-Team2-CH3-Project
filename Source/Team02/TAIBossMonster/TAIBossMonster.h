#pragma once
#include "Character/TCharacterBase.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TAIBossMonster.generated.h"

DECLARE_DELEGATE_TwoParams(FOnAttackMontageEnded,UAnimMontage*,bool)

UCLASS()
class TEAM02_API ATAIBossMonster : public ATCharacterBase
{
	GENERATED_BODY()

public:
	ATAIBossMonster();

	
	
	
protected:
	virtual void BeginPlay() override;
	//virtual void BeginAttack();
	//virtual void EndAttack(UAnimMontage* InMontage,bool bInterruped);

public:
	bool bIsNowAttacking;

protected:
	FOnAttackMontageEnded OnAttackMontageEndedDelegate;
	
};
