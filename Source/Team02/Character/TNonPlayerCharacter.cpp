#include "Character/TNonPlayerCharacter.h"
#include "AI/TAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/TAnimInstance.h"
#include "Item/TGunNPCWeapon.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Team02.h"


int32 ATNonPlayerCharacter::ShowGunAttackDebug = 0;

FAutoConsoleVariableRef CVarShowGunAttackDebug(
	TEXT("TAI.ShowGunAttackDebug"),
	ATNonPlayerCharacter::ShowGunAttackDebug,
	TEXT(""),
	ECVF_Cheat
	);

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

void ATNonPlayerCharacter::AttachWeapon(TSubclassOf<ATGunNPCWeapon> Weapon)
{
	if (Weapon)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		CurrentRifle = GetWorld()->SpawnActor<ATGunNPCWeapon>(Weapon, SpawnParams);

		//부착 규칙
		const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		
		if (CurrentRifle)
		{
			//소켓에 부착
			CurrentRifle->AttachToComponent(GetMesh(), AttachmentRules, FName("weapon_r_muzzle"));
			CurrentRifle->SetActorEnableCollision(false);

			//총의 물리 피직스 끄기
			UPrimitiveComponent* WeaponRoot = Cast<UPrimitiveComponent>(CurrentRifle->GetRootComponent());
			if (WeaponRoot)
			{
				WeaponRoot->SetSimulatePhysics(false);
			}
				
		}
	}
}

void ATNonPlayerCharacter::BeginAttack()
{
	
	UTAnimInstance* AnimInstance = Cast<UTAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance."));
	
	if (IsValid(AnimInstance) == true&& IsValid(AttackFireMontage) == true && AnimInstance->Montage_IsPlaying(AttackFireMontage) == false)
	{
		AnimInstance->Montage_Play(AttackFireMontage);

		bIsNowAttacking = true;

		if (OnAttackMontageEndedDelegate.IsBound() == false)
		{
			AnimInstance->Montage_Play(AttackFireMontage);
		
			//몽타주 종료
			OnAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
			AnimInstance->Montage_SetEndDelegate(OnAttackMontageEndedDelegate, AttackFireMontage);
		}
	}
}

float ATNonPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		ATAIController* AIController = Cast<ATAIController>(GetController());
		if (IsValid(AIController) == true)
		{
			AIController->EndAI();
		}
	}
	
	return FinalDamageAmount;
}


void ATNonPlayerCharacter::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	bIsNowAttacking = false;

	if (OnAttackMontageEndedDelegate.IsBound() == true)
	{
		//바인딩 했던 함수 해재
		OnAttackMontageEndedDelegate.Unbind();
	}
}

void ATNonPlayerCharacter::HandleOnCheckHit()
{
	if (!IsValid(CurrentRifle))
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Weapon is not valid."));
		return;
	}
	
	//무기 메시에서 MuzzleFlash 위치 가져오기
	const FName MuzzleSocketName = TEXT("MuzzleFlash");
	FVector StartLocation = CurrentRifle->GetMesh()->GetSocketLocation(MuzzleSocketName);
	FVector EndLocation = GetActorLocation() + (GetActorForwardVector() * CurrentRifle->GetMaxAttackRange());
	
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	Params.AddIgnoredActor(CurrentRifle);
	Params.AddIgnoredActor(this);
	
	bool bResult = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_ATTACK,
		Params
	);

	if (bResult == false)
	{
		HitResult.TraceStart = StartLocation;
		HitResult.TraceEnd = EndLocation;
	}

	if (bResult == true)
	{
		ATCharacterBase* HittedCharacter = Cast<ATCharacterBase>(HitResult.GetActor());
		if (IsValid(HittedCharacter) == true)
		{
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));
			FDamageEvent DamageEvent;
			HittedCharacter->TakeDamage(
				10.f,
				DamageEvent,
				GetController(),
				this
				);
		}
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) == true)
	{
		if (AnimInstance->Montage_IsPlaying(AttackFireMontage) == false)
		{
			AnimInstance->Montage_Play(AttackFireMontage);
		}
	}

	if (1 == ShowGunAttackDebug)
	{
		if (bResult == true)
		{
			DrawDebugLine(
				GetWorld(),
				StartLocation,
				HitResult.ImpactPoint,
				FColor::Blue,
				false,
				5.f,
				0,
				2.f
				);
		}
		else
		{
			DrawDebugLine(
				GetWorld(),
				StartLocation,
				EndLocation,
				FColor::Blue,
				false,
				5.f,
				0,
				2.f
			);
		}
	}
}



