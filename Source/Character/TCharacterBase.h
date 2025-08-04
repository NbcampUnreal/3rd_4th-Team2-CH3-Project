#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TCharacterBase.generated.h"

class UAnimMontage;

UCLASS()
class TEAM02_API ATCharacterBase : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

#pragma region HP
	
public:
	ATCharacterBase();

	float GetMaxHP() const { return MaxHP; }

	float GetCurrentHP() const { return CurrentHP; }

	void SetMaxHP(float InMaxHP) { MaxHP = InMaxHP; }

	void SetCurrentHP(float InCurrentHP) { CurrentHP = InCurrentHP; }

	bool IsDead() const { return bIsDead; }

	// 테스트용 데미지 함수
	virtual void TakeDamage(float Damage);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CurrentHP = 100.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsDead : 1;

#pragma endregion

#pragma region Attack

public:
	UFUNCTION()
	void HandleOnCheckHit();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackFireMontage;

#pragma endregion
	
};
