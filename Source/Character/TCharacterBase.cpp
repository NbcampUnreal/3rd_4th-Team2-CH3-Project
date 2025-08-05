#pragma once

#include "Character/TCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Animation/TAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/EngineTypes.h"
#include "Animation/TAnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"

ATCharacterBase::ATCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	float CharacterHalfHeight = 90.f;
	float CharacterRadius = 40.f;

	GetCapsuleComponent()->InitCapsuleSize(CharacterRadius, CharacterHalfHeight);

	FVector PivotPosition(0.f, 0.f, -CharacterHalfHeight);
	FRotator PivotRotation(0.f, -90.f, 0.f);
	GetMesh()->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);
	
	//콜리전 프리셋을 SXCharacterMesh로 설정 - nam
	GetMesh()->SetCollisionProfileName(TEXT("SXCharacterMesh"));

	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	bIsDead = false;
}

void ATCharacterBase::HandleOnCheckHit()
{
	UKismetSystemLibrary::PrintString(this, TEXT("HandleOnCheckHit())"));
}

// 테스트용 함수
float ATCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return FinalDamageAmount;
}

void ATCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ATCharacterBase::HandleOnCheckInputAttack()
{
	UTAnimInstance* AnimInstance = Cast<UTAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance"));

}

void ATCharacterBase::BeginAttack()
{
	UTAnimInstance* AnimInstance = Cast<UTAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance."));
	
	bIsNowAttacking = true;
	if (IsValid(AnimInstance) == true&& IsValid(AttackFireMontage) == true && AnimInstance->Montage_IsPlaying(AttackFireMontage) == false)
	{
		AnimInstance->Montage_Play(AttackFireMontage);
	}

	if (OnNormalAttackMontageEndedDelegate.IsBound() == false)
	{
		OnNormalAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
		AnimInstance->Montage_SetEndDelegate(OnNormalAttackMontageEndedDelegate, AttackFireMontage);
	}
}

void ATCharacterBase::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	bIsAttackKeyPressed = false;
	bIsNowAttacking = false;

	if (OnNormalAttackMontageEndedDelegate.IsBound() == true)
	{
		//바인딩 했던 함수 해재
		OnNormalAttackMontageEndedDelegate.Unbind();
	}
}

