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
