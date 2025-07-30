#include "Character/TNonPlayerCharacter.h"
#include "AI/TAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

ATNonPlayerCharacter::ATNonPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	//npc 컨트롤 가져오기
	AIControllerClass = ATAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ATNonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	//플레이어 캐릭터가 아니라면
	if (false == IsPlayerControlled())
	{
		bUseControllerRotationYaw = false;
		//NPC의 회전 부드러움 적용
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
		//NPC의 최고속도
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
	}
}
