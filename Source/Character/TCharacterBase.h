#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class TEAM02_API ATCharacterBase : public ACharacter
{
	GENERATED_BODY()

#pragma region HP
	
public:
	ATCharacterBase();

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

#pragma endregion
};
