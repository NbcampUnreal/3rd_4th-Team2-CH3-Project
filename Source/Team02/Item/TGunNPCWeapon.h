#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGunNPCWeapon.generated.h"

class ATNonPlayerCharacter;
class UStaticMeshComponent;

UCLASS()
class TEAM02_API ATGunNPCWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ATGunNPCWeapon();

	float GetMaxAttackRange() const { return MaxShotAttackRange; }

	UFUNCTION(Blueprintable)
	UStaticMeshComponent* GetMesh() const { return StaticMeshComp; }
	

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;

	//총기 사거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Units = cm))
	float MaxShotAttackRange = 20000.f;
};
