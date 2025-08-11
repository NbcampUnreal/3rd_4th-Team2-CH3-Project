#include "Team02/Character/TNonPlayerCharacterSword.h"
#include "Team02/Controller/TSwordAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/TAnimInstance.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Team02.h"

int32 ATNonPlayerCharacterSword::SwordAttackSwordDebug = 0;

FAutoConsoleVariableRef CVarSwordAttackSwordDebug(
	TEXT("TAI.SwordAttackDebug"),
	ATNonPlayerCharacterSword::SwordAttackSwordDebug,
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

		AttachWeapon(Sword);
	}
}

void ATNonPlayerCharacterSword::AttachWeapon(TSubclassOf<AActor> Weapon)
{
	if (Weapon)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		CurrentSword = GetWorld()->SpawnActor<AActor>(Weapon, SpawnParams);

		//부착 규칙
		const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		
		if (CurrentSword)
		{
			//소켓에 부착
			CurrentSword->AttachToComponent(GetMesh(), AttachmentRules, FName("hand_rSocket"));
			CurrentSword->SetActorEnableCollision(false);

			//총의 물리 피직스 끄기
			UPrimitiveComponent* WeaponRoot = Cast<UPrimitiveComponent>(CurrentSword->GetRootComponent());
			if (WeaponRoot)
			{
				WeaponRoot->SetSimulatePhysics(false);
			}
				
		}
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
			CurrentSword->Destroy();
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

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepMultiByChannel(
	HitResults, 
	GetActorLocation(), 
	GetActorLocation() + AttackSwordRange * GetActorForwardVector(), 
	FQuat::Identity, 
	ECC_ATTACK, 
	FCollisionShape::MakeSphere(AttackSwordRadius),
	Params
	);

	if (true == bResult)
	{
		if (HitResults.IsEmpty() == false)
		{
			for (FHitResult HitResult : HitResults)
			{
				if (IsValid(HitResult.GetActor()) == true)
				{
					if (1 == SwordAttackSwordDebug)
					{
						UKismetSystemLibrary::PrintString(
						this,FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));
					}
				}
			}
		}
	}

	if (1 == SwordAttackSwordDebug)
	{
		FVector TraceVector = AttackSwordRange * GetActorForwardVector();
		FVector Center = GetActorLocation() + TraceVector + GetActorUpVector() * 40.f;
		float HalfHeight = AttackSwordRange * 0.5f + AttackSwordRadius;
		FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVector).ToQuat();
		FColor DrawColor = true == bResult ? FColor::Green : FColor::Red;
		float DebugLifeTime = 5.f;

		//디버깅 목적으로 게임 월드에 캡슐(Capsule) 형태의 선을 그리는 언리얼 엔진의 함수
		DrawDebugCapsule(
			GetWorld(),
			Center,
			HalfHeight,
			AttackSwordRadius,
			CapsuleRot,
			DrawColor,
			false,
			DebugLifeTime
		);
	}
}


