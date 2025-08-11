#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TSwordNPCWeapon.generated.h"

class ATNonPlayerCharacterSword;
class UStaticMeshComponent;

UCLASS()
class TEAM02_API ATSwordNPCWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ATSwordNPCWeapon();

	float GetMaxAttackRange() const { return MaxShotAttackRange; }

	UFUNCTION(Blueprintable)
	UStaticMeshComponent* GetMesh() const { return StaticMeshComp; }
	

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;

	//총기 사거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Units = cm))
	float MaxShotAttackRange = 200.f;

};
