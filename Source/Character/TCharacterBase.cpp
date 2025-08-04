#pragma once

#include "Character/TCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	bIsDead = false;
}

void ATCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATCharacterBase::HandleOnCheckHit()
{
	UKismetSystemLibrary::PrintString(this, TEXT("HandleOnCheckHit())"));
}

// 테스트용 함수
void ATCharacterBase::TakeDamage(float Damage)
{
	SetCurrentHP(CurrentHP - Damage);
	UE_LOG(LogTemp,Warning,TEXT("Current HP: %f"),CurrentHP);
}