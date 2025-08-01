#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TCharacterBase.generated.h"

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

	//체력테스트 하려고 한 부분이라 주석처리 or 삭제하셔도 됩니다!
	//삭제하기전에 레벨블루프린트에 있는 연결부분 삭제해주세요!
	UFUNCTION(BlueprintCallable,Category="HP")
	void TakeDamage(float Damage);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CurrentHP = 100.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsDead : 1;

#pragma endregion
};
