#pragma once

#include "Character/TCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Animation/TAnimInstance.h"
#include "Engine/EngineTypes.h"
#include "Item/TWeaponBase.h"
#include "GameFramework/CharacterMovementComponent.h"

ATCharacterBase::ATCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	float CharacterHalfHeight = 90.f;
	float CharacterRadius = 40.f;

	GetCapsuleComponent()->InitCapsuleSize(CharacterRadius, CharacterHalfHeight);

	FVector PivotPosition(0.f, 0.f, -CharacterHalfHeight);
	FRotator PivotRotation(0.f, -90.f, 0.f);
	GetMesh()->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);
	GetMesh()->SetCollisionProfileName(TEXT("TCharacterMesh"));
	
	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.1f;

	CurrentWeaponType = EWeaponType::Unarmed;
	
	bIsDead = false;
}

void ATCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	UTAnimInstance* AnimInstance = Cast<UTAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance) == true)
	{
		AnimInstance->OnPostDead.AddDynamic(this, &ThisClass::HandleOnPostCharacterDead);
	}
}

void ATCharacterBase::EquipWeapon(ATWeaponBase* NewWeapon)
{
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->Destroy();
	}
	CurrentWeapon = NewWeapon;

	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Hand_R_Socket"));
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->SetActorEnableCollision(false);

		CurrentWeaponType = CurrentWeapon->WeaponType;
	}
	else
	{
		CurrentWeaponType = EWeaponType::Unarmed;
	}
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
	if (IsValid(AnimInstance) == true&& IsValid(GetCurrentWeaponAttackAnimMontage()) == true && AnimInstance->Montage_IsPlaying(GetCurrentWeaponAttackAnimMontage()) == false)
	{
		AnimInstance->Montage_Play(GetCurrentWeaponAttackAnimMontage());
	}

	if (OnNormalAttackMontageEndedDelegate.IsBound() == false)
	{
		OnNormalAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
		AnimInstance->Montage_SetEndDelegate(OnNormalAttackMontageEndedDelegate, GetCurrentWeaponAttackAnimMontage());
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

UAnimMontage* ATCharacterBase::GetCurrentWeaponAttackAnimMontage() const
{
	if (IsValid(CurrentWeapon) == true)
	{
		return CurrentWeapon->GetAttackMontage();
	}
	return nullptr;
}

float ATCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHP = FMath::Clamp(CurrentHP - FinalDamageAmount, 0.f, MaxHP);

	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		bIsDead = true;
		CurrentHP = 0.f;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}
	return FinalDamageAmount;
}

void ATCharacterBase::HandleOnPostCharacterDead()
{
	SetLifeSpan(0.1f);
}


