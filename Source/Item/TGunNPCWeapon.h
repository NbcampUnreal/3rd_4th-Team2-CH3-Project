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

	

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
};
