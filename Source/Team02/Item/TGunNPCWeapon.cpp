#include "Item/TGunNPCWeapon.h"
#include "Components/StaticMeshComponent.h"

ATGunNPCWeapon::ATGunNPCWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NPCRifle"));
	SetRootComponent(StaticMeshComp);
	
}
