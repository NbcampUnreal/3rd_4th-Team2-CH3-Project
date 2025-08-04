#pragma once

#include "Character/TCharacterBase.h"
#include "Components/CapsuleComponent.h"
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

	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	bIsDead = false;
}
// 체력 부분 테스트할려고 함수 삽입 나중에 주석처리하거나 삭제하셔도 되요! 삭제하기전에 레벨블루프린트에 있는노드 삭제해주세요!
void ATCharacterBase::TakeDamage(float Damage)
{
	SetCurrentHP(CurrentHP - Damage);
	UE_LOG(LogTemp,Warning,TEXT("Current HP = %f"),CurrentHP);
}
