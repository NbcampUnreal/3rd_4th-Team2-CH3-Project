#include "Team02/Item/TSwordNPCWeapon.h"

ATSwordNPCWeapon::ATSwordNPCWeapon()
{
 	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NPCRifle"));
	SetRootComponent(StaticMeshComp);
}



