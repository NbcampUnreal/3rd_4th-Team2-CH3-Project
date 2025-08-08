#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TAnimInstance.generated.h"

enum class EWeaponType : uint8;

class ATCharacterBase;
class UCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPostDead);

UCLASS()
class TEAM02_API UTAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UFUNCTION()
	void AnimNotify_CheckHit();
	UFUNCTION()
	void AnimNotify_PostDead();

public:
	FOnCheckHit OnCheckHit;

	FOnPostDead OnPostDead;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ATCharacterBase> OwnerCharacter;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovement;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint8 bShouldMove : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsDead : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bIsFalling : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector Velocity;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly)
	float NormalizedCurrentPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	EWeaponType WeaponType;
};
