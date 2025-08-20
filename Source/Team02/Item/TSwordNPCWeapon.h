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

	UFUNCTION(Blueprintable)
	UStaticMeshComponent* GetMesh() const { return StaticMeshComp; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
};
