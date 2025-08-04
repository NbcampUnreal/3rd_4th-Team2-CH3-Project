#include "Team02/TAIBossMonster/TAIBossMonster.h"
#include "GameFramework/CharacterMovementComponent.h"

ATAIBossMonster::ATAIBossMonster()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ATAIBossMonster::BeginPlay()
{
	Super::BeginPlay();

	if (false==IsPlayerControlled())
	{
		bUseControllerRotationYaw = false;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);

		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		
	}
}

