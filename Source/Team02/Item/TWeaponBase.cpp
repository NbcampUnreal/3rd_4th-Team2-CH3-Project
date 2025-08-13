// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TWeaponBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/TPlayerCharacter.h"
#include "Character/TCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "Team02.h"
#include "NiagaraComponent.h" 

ATWeaponBase::ATWeaponBase()
{
	Damage = 20.0f;
	MaxAmmo = 30;
	TotalAmmo = 180;
	CurrentAmmo = 30;
	FireRate = 0.2f;
	ReloadTime = 1.5f;
	Range = 2000.0f;

	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(StaticMesh); // 총구 기준 잡고 싶으면 WeaponMesh에 Attach
}

void ATWeaponBase::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// OtherActor가 캐릭터, 관전자, 혹은 플레이어 컨트롤러가 Possess한 Pawn인지 체크
	if (OtherActor && OtherActor != this)
	{
		// 예시: 플레이어가 SpectatorPawn이면, SpectatorPlayerController를 가져와서 무기 소유 변수에 할당
		// 소유자 컨트롤러/캐릭터 구하기
		ATPlayerCharacter* PC = Cast<ATPlayerCharacter>(OtherActor);
		if (PC)
		{
			PC->EquipWeapon(this);
		}
	}
}


void ATWeaponBase::Fire()
{
	if (CanFire())
	{
		FVector MuzzleLoc = MuzzlePoint->GetComponentLocation();
		FRotator MuzzleRot = MuzzlePoint->GetComponentRotation();
		FVector TraceStart = MuzzleLoc;
		FVector TraceEnd = TraceStart + (MuzzleRot.Vector() * Range);

		// 트레이스 파라미터
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this); // 자기 자신 무시
		if (GetOwner())
			Params.AddIgnoredActor(GetOwner()); // 무기 소유자 무시(선택)

		// 라인 트레이스!
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			TraceStart,
			TraceEnd,
			ECC_ATTACK,
			Params
		);

		DrawDebugLine(
			GetWorld(),
			TraceStart,
			TraceEnd,
			FColor::Red, // 선 색상 (빨간색)
			false, // true면 계속, false면 잠깐
			1.0f, // 지속시간(초)
			0, // 두께 그룹
			2.0f // 선 두께
		);
		// 피격 처리
		if (bHit)
		{
			// 데미지 적용
			if (HitResult.GetActor())
			{
				UGameplayStatics::ApplyPointDamage(
					HitResult.GetActor(),
					Damage,
					MuzzleRot.Vector(),
					HitResult,
					GetOwner() ? GetOwner()->GetInstigatorController() : nullptr,
					this,
					nullptr // 데미지 타입(기본)
				);
			}
			// 피격 이펙트, 사운드 등 추가 가능
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit: %s"), *HitResult.GetActor()->GetName()));
		}
		else
		{
			UKismetSystemLibrary::PrintString(this, TEXT("Miss!"));
		}

		// 이펙트(트레이서, 총구불빛 등) 추가하면 여기서

		// 탄약 차감, 쿨타임 관리
		bCanFire = false;
		GetWorld()->GetTimerManager().SetTimer(
			FireRateTimerHandle,
			this, &ATWeaponBase::ResetCanFire,
			FireRate, false
		);
		SetCurrentAmmo(GetCurrentAmmo() - 1);
	}
	else if (!bCanFire)
	{
		//UKismetSystemLibrary::PrintString(this, TEXT("Fire(): FireRate!!"));
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Fire(): No ammo!"));
	}
}

void ATWeaponBase::FireFrom(FVector Start, FVector FireDir)
{
    if (!CanFire()) return;

    // 0) Owner/Controller
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    AController* Ctrl = OwnerPawn ? OwnerPawn->GetController() : nullptr;

    // 1) 카메라(또는 Eyes)로 조준 "방향"만 얻기
    FVector CamLoc; FRotator CamRot;
    if (Ctrl)        Ctrl->GetPlayerViewPoint(CamLoc, CamRot);
    else if (OwnerPawn) OwnerPawn->GetActorEyesViewPoint(CamLoc, CamRot);
    else             { CamLoc = Start; CamRot = FRotator::ZeroRotator; }

    // 카메라가 본 지점 (없으면 카메라 전방으로 Range만큼)
    const FVector CamEnd = CamLoc + CamRot.Vector() * Range;
    FCollisionQueryParams Q; Q.AddIgnoredActor(this); if (OwnerPawn) Q.AddIgnoredActor(OwnerPawn);
    FHitResult CamHit;
    const bool bCamHit = GetWorld()->LineTraceSingleByChannel(CamHit, CamLoc, CamEnd, ECC_ATTACK, Q);
    FVector AimPoint = bCamHit ? CamHit.ImpactPoint : CamEnd;

    // 너무 가까운 목표 보정(패럴랙스 완화)
    const float MinAimDist = 200.f;
    const float DistFromCam = (AimPoint - CamLoc).Size();
    if (DistFromCam < MinAimDist)
        AimPoint = CamLoc + CamRot.Vector() * MinAimDist;

    // 2) 최종 발사 방향
    const FVector Dir = (AimPoint - Start).GetSafeNormal();

    // 3) 실제 판정 라인트레이스: 사거리 고정(Start + Dir * Range)
	const FVector TraceEnd = Start + Dir * Range;
	FHitResult HitResult;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, TraceEnd, ECC_ATTACK, Q);

	// ⬇️ 이 길이를 이펙트에 그대로 넘기면 “뚫고 가지 않음”
	const FVector FinalPoint = bHit ? HitResult.ImpactPoint : TraceEnd;
	float BeamLength = (FinalPoint - Start).Size();  // 히트면 충돌 지점까지, 미스면 사거리
    // 4) FX/사운드/데미지
    //    - 트레이서는 항상 Range 길이
    FireEffect(Start, const_cast<FVector&>(Dir), BeamLength); // Length=Range로 세팅하도록 구현:contentReference[oaicite:1]{index=1}
    FireSounds(Start);                                                       // 위치 넘겨 재생:contentReference[oaicite:2]{index=2}

    if (bHit && HitResult.GetActor())
    {
        UGameplayStatics::ApplyPointDamage(
            HitResult.GetActor(), Damage, Dir, HitResult,
            OwnerPawn ? OwnerPawn->GetController() : nullptr,
            this, nullptr);                                                  //:contentReference[oaicite:3]{index=3}
    }
    else
    {
        UKismetSystemLibrary::PrintString(this, TEXT("Miss!"));
    }

    // 5) 쿨타임/탄약
    bCanFire = false;
    GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &ATWeaponBase::ResetCanFire, FireRate, false); //:contentReference[oaicite:4]{index=4}
    SetCurrentAmmo(GetCurrentAmmo() - 1); 

	// 카메라선(빨강) — 얇게
	DrawDebugLine(GetWorld(), CamLoc, bCamHit ? CamHit.ImpactPoint : CamEnd,
				  FColor::Red,  false, /*Life*/0.35f, /*Depth*/0, /*Thickness*/0.6f);

	// 머즐→사거리(파랑) — 얇게
	DrawDebugLine(GetWorld(), Start, TraceEnd,
				  FColor::Blue, false, 0.35f, 0, 0.6f);

	// 히트 지점(노랑) — 구체 반지름도 축소
	DrawDebugSphere(GetWorld(), bHit ? HitResult.ImpactPoint : TraceEnd,
					/*Radius*/4.f, /*Segments*/10, FColor::Yellow,
					false, 0.35f);
    // --- (디버그 원하면 주석 해제) ---
    // DrawDebugLine(GetWorld(), CamLoc, AimPoint, FColor::Red,   false, 1.5f, 0, 2.f);  // 카메라선
    // DrawDebugLine(GetWorld(), Start,  FinalPoint, FColor::Blue, false, 1.5f, 0, 2.f);  // 머즐선(판정/이펙트와 동일)
    // DrawDebugSphere(GetWorld(), FinalPoint, 6.f, 12, FColor::Yellow, false, 1.5f);
}

void ATWeaponBase::Reload()
{
	int32 NeedAmmo = MaxAmmo - GetCurrentAmmo();

	if (NeedAmmo <= 0)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Reload(): Already Full!"));
		return;
	}
	if (GetTotalAmmo() <= 0)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Reload(): No Ammo!"));
		return;
	}

	int32 AmmoToReload = FMath::Min(NeedAmmo, GetTotalAmmo());

	SetCurrentAmmo(GetCurrentAmmo() + AmmoToReload);
	SetTotalAmmo(GetTotalAmmo() - AmmoToReload);

	FString Msg = FString::Printf(
		TEXT("Reloaded: %d | Current: %d | Remain: %d"), AmmoToReload, GetCurrentAmmo(), GetTotalAmmo());
	UKismetSystemLibrary::PrintString(this, Msg);

	if (ReloadMontage) // Reload시에 재생하는 몽타주 구현
	{
		if (ATPlayerCharacter* PlayerCharacter = Cast<ATPlayerCharacter>(GetOwner()))
		{
			UAnimInstance* AnimInstance = PlayerCharacter->GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(ReloadMontage);
			}
		}
	}
}

bool ATWeaponBase::CanFire() const
{
	return CurrentAmmo > 0 && bCanFire;
}

bool ATWeaponBase::CanReload() const
{
	return (GetCurrentAmmo() < MaxAmmo) && (GetTotalAmmo() > 0);
}

void ATWeaponBase::Equip()
{
	UKismetSystemLibrary::PrintString(this, TEXT("Equip() called!"));
	// TODO: 무기 장착 연출/효과 등
}

void ATWeaponBase::Unequip()
{
	UKismetSystemLibrary::PrintString(this, TEXT("Unequip() called!"));
	// TODO: 무기 해제 연출/효과 등
}

FString ATWeaponBase::GetWeaponTypeString() const
{
	switch (WeaponType)
	{
	case EWeaponType::Shotgun: return TEXT("Shotgun");
	case EWeaponType::Rifle: return TEXT("Rifle");
	case EWeaponType::Pistol: return TEXT("Pistol");
	// 필요에 따라 추가
	default: return TEXT("Unknown");
	}
}

void ATWeaponBase::ResetCanFire()
{
	bCanFire = true;
}

UAnimMontage* ATWeaponBase::GetAttackMontage()
{
	return AttackMontage;
}


void ATWeaponBase::FireEffect(FVector& MuzzleLoc, FVector& MuzzleRot, float& BeamLength) const
{
	// Niagara 빔 스폰 + 사용자 파라미터 설정
	if (BeamFlashFX)
	{
		
		UNiagaraComponent* BeamComp =
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(), BeamFlashFX, MuzzleLoc, MuzzleRot.Rotation());

		if (BeamComp)
		{
			// 사용자 파라미터 이름이 "Length" 라고 가정
			BeamComp->SetVectorParameter(FName("Length"),
										 FVector(BeamLength, 0.f, 0.f));
		}
	}
}

void ATWeaponBase::FireSounds(FVector& MuzzleLoc)
{
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, MuzzleLoc);
	}
}
