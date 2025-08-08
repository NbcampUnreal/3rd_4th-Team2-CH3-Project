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
	CurrentWeapon = nullptr;
	bIsDead = false;

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
	if (NewWeapon == nullptr)
	{
		if (IsValid(CurrentWeapon))
		{
			CurrentWeapon->Destroy();
		}
		CurrentWeapon = nullptr;
		CurrentWeaponType = EWeaponType::Unarmed;
		return;
	}
	
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->Destroy();
	}
	
	CurrentWeapon = NewWeapon;
	
	CurrentWeaponType = CurrentWeapon->WeaponType;
	
	FName SocketToAttach = NAME_None;
	
	switch (CurrentWeaponType)
	{
	case EWeaponType::Pistol:
		SocketToAttach = TEXT("Hand_R_Socket");
		break;
	case EWeaponType::Rifle:
	case EWeaponType::Shotgun:
		SocketToAttach = TEXT("Rifle_Socket");
		break;
	}
	if (SocketToAttach != NAME_None)
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketToAttach);
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->SetActorEnableCollision(false);
	}
}

ATWeaponBase* ATCharacterBase::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

EWeaponType ATCharacterBase::GetCurrentWeaponType() const
{
	if (IsValid(CurrentWeapon))
	{
		return CurrentWeapon->WeaponType;
	}
	return EWeaponType::Unarmed;
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


