#include "Team02/Character/TNonPlayerCharacterSword.h"
#include "Team02/Controller/TSwordAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/TAnimInstance.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Team02.h"

int32 ATNonPlayerCharacterSword::SwordAttackDebug = 0;

FAutoConsoleVariableRef CVarSwordAttackDebug(
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

void ATNonPlayerCharacterSword::BeginAttack()
{
	
	UTAnimInstance* AnimInstance = Cast<UTAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance."));
	
	if (IsValid(AnimInstance) == true&& IsValid(AttackSwordMontage) == true && AnimInstance->Montage_IsPlaying(AttackSwordMontage) == false)
	{
		AnimInstance->Montage_Play(AttackSwordMontage);

		bIsNowAttacking = true;

		if (OnSwordAttackMontageEndedDelegate.IsBound() == false)
		{
			AnimInstance->Montage_Play(AttackSwordMontage);
		
			//몽타주 종료
			OnSwordAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
			AnimInstance->Montage_SetEndDelegate(OnSwordAttackMontageEndedDelegate, AttackSwordMontage);
		}
	}
}

float ATNonPlayerCharacterSword::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		ATSwordAIController* AIController = Cast<ATSwordAIController>(GetController());
		if (IsValid(AIController) == true)
		{
			AIController->EndAI();
			//CurrentRifle->SetLifeSpan(0.7f);
		}
	}
	
	return FinalDamageAmount;
}

void ATNonPlayerCharacterSword::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	bIsNowAttacking = false;

	if (OnSwordAttackMontageEndedDelegate.IsBound() == true)
	{
		//바인딩 했던 함수 해재
		OnSwordAttackMontageEndedDelegate.Unbind();
	}
}

void ATNonPlayerCharacterSword::HandleOnCheckSwordHit()
{
	UKismetSystemLibrary::PrintString(this, TEXT("HandleOnCheckSwordHit()"));
}

