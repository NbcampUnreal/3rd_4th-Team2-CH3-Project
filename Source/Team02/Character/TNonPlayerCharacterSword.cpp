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

void ATNonPlayerCharacterSword::BeginPlay()
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
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
		
	}
}