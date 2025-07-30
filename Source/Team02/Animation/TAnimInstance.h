#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TAnimInstance.generated.h"

class ATCharacterBase;
class ATNonPlayerCharacter;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class TEAM02_API UTAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ATCharacterBase> OwnerCharacter;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector Velocity;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GroundSpeed;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bShouldMove : 1;
	
	//오너 캐릭터 선언
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ATNonPlayerCharacter> NPCCharacter;

	//오너 캐릭터의 이동 컴포넌트 정의
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> NPCMovement;
	
	//점프중인지 확인
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bIsFalling : 1;
};
