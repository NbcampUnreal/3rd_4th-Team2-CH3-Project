#include "Team02/Character/TNonPlayerCharacterSword.h"
#include "Team02/Controller/TSwordAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/TAnimInstance.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Team02.h"

int32 ATNonPlayerCharacterSword::SwordAttackDebug = 0;

FAutoConsoleVariableRef CVarShowGunAttackDebug(
	TEXT("TAI.SwordAttackDebug"),
	ATNonPlayerCharacterSword::SwordAttackDebug,
	TEXT(""),
	ECVF_Cheat
	);

ATNonPlayerCharacterSword::ATNonPlayerCharacterSword()
	: bIsNowAttacking(false)
{
	PrimaryActorTick.bCanEverTick = true;
	//건 npc 컨트롤 가져오기
	AIControllerClass = ATSwordAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}