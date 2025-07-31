#include "Character/TNonPlayerCharacter.h"
#include "AI/TAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/TAnimInstance.h"
#include "Item/TGunNPCWeapon.h"
#include "Engine/SkeletalMeshSocket.h"

ATNonPlayerCharacter::ATNonPlayerCharacter()
	: bIsNowAttacking(false)
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

		AttachWeapon(Rifle);
	}
}

void ATNonPlayerCharacter::AttachWeapon(TSubclassOf<ATGunNPCWeapon> Weapon) const
{
	if (Weapon)
	{
		//무기 스폰
		AActor* SpawnWeapon = GetWorld()->SpawnActor<ATGunNPCWeapon>(Weapon);

		//부착 규칙
		const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		
		if (SpawnWeapon)
		{
			//소켓에 부착
			SpawnWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("weapon_r_muzzle"));
			SpawnWeapon->SetActorEnableCollision(false);

			//총의 물리 피직스 끄기
			UPrimitiveComponent* WeaponRoot = Cast<UPrimitiveComponent>(SpawnWeapon->GetRootComponent());
			if (WeaponRoot)
			{
				WeaponRoot->SetSimulatePhysics(false);
			}
				
		}
	}
}

void ATNonPlayerCharacter::BeginAttack()
{
	if (GetCharacterMovement()->IsFalling() == true)
	{
		return;
	}

	UTAnimInstance* AnimInstance = Cast<UTAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance."));

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	if (IsValid(AnimInstance) == true &&
		IsValid(AttackFireMontage) == true &&
		AnimInstance->Montage_IsPlaying(AttackFireMontage) == false)
	{
		AnimInstance->Montage_Play(AttackFireMontage);

		bIsNowAttacking = true;
	}
}

void ATNonPlayerCharacter::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	bIsNowAttacking = false;
}

