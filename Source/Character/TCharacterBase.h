#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TCharacterBase.generated.h"

class ATWeaponBase;

UCLASS()
class TEAM02_API ATCharacterBase : public ACharacter
{
	GENERATED_BODY()
	
#pragma region Override ACharacter
	
public:
	ATCharacterBase();
	
protected:
	virtual void BeginPlay() override;
	
#pragma endregion
	
#pragma region HP
	
public:
	float GetMaxHP() const { return MaxHP; }

	float GetCurrentHP() const { return CurrentHP; }

	void SetMaxHP(float InMaxHP) { MaxHP = InMaxHP; }

	void SetCurrentHP(float InCurrentHP) { CurrentHP = InCurrentHP; }

	bool IsDead() const { return bIsDead; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CurrentHP = 100.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsDead : 1;
	
	UFUNCTION()
	virtual void HandleOnPostCharacterDead();

#pragma endregion
	
#pragma region Attack

public:
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	void HandleOnCheckInputAttack();
	
	virtual void BeginAttack();

	UAnimMontage* GetCurrentWeaponAttackAnimMontage() const;
	
	UFUNCTION()
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped);
	
	UPROPERTY()
	TObjectPtr<ATWeaponBase> CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponType CurrentWeaponType;
	
	UFUNCTION(BlueprintPure, Category = "Animation")
	EWeaponType GetCurrentWeaponType() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	ATWeaponBase* GetCurrentWeapon() const;
    
	void EquipWeapon(ATWeaponBase* NewWeapon);

protected:
	FString AttackAnimMontageSectionPrefix = FString(TEXT("Attack"));

	int32 MaxComboCount = 3;

	int32 CurrentComboCount = 0;

	bool bIsNowAttacking = false;

	bool bIsAttackKeyPressed = false;
	
	//라이플 공격하는 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackFireMontage;

	FOnMontageEnded OnNormalAttackMontageEndedDelegate;

#pragma endregion
};
