// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TWeaponBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/TPlayerCharacter.h"
#include "Character/TCharacterBase.h" 
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Team02.h"

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
			// 기존 무기 파괴
			if (PC->CurrentWeapon)
			{
				PC->CurrentWeapon->Destroy();
			}

			// 새 무기 등록
			PC->CurrentWeapon = this;

			// 손에 붙이기
			AttachToComponent(
				PC->GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				TEXT("Hand_R_Socket")
			);
			SetActorHiddenInGame(false);          // <<<<<<<<<< 꼭 해줘야 함!
			SetActorEnableCollision(false);
		}
		// 추가: 효과음, UI 표시, 인벤토리 추가 등
		
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
		FColor::Red,   // 선 색상 (빨간색)
		false,         // true면 계속, false면 잠깐
		1.0f,          // 지속시간(초)
		0,             // 두께 그룹
		2.0f           // 선 두께
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

    FVector TraceEnd = Start + (FireDir * Range);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    if (GetOwner())
        Params.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult, Start, TraceEnd, ECC_ATTACK, Params);

    // (옵션) 디버그 선
    DrawDebugLine(GetWorld(), Start, TraceEnd, FColor::Red, false, 1.0f, 0, 2.0f);

    if (bHit && HitResult.GetActor())
    {

        // 데미지 전달 (TakeDamage로 자동 호출)
        UGameplayStatics::ApplyPointDamage(
            HitResult.GetActor(),
            Damage,                     // float: 데미지량
            FireDir,                    // FVector: 방향
            HitResult,                  // FHitResult: 피격 정보
            GetOwner() ? GetOwner()->GetInstigatorController() : nullptr,
            this,                       // DamageCauser: 누가 쐈는지
            nullptr                     // DamageTypeClass(기본 null)
        );

        // (옵션) 디버그 출력
        if (ATCharacterBase* HitChar = Cast<ATCharacterBase>(HitResult.GetActor()))
        {
            UKismetSystemLibrary::PrintString(
                this, FString::Printf(TEXT("Hit: %s / HP: %.1f"),
                *HitChar->GetName(), HitChar->GetCurrentHP()));
        }
        else
        {
            UKismetSystemLibrary::PrintString(
                this, FString::Printf(TEXT("Hit: %s"), *HitResult.GetActor()->GetName()));
        }
    }
    else
    {
        UKismetSystemLibrary::PrintString(this, TEXT("Miss!"));
    }

    bCanFire = false;
    GetWorld()->GetTimerManager().SetTimer(
        FireRateTimerHandle,
        this, &ATWeaponBase::ResetCanFire,
        FireRate, false
    );
    SetCurrentAmmo(GetCurrentAmmo() - 1);
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

	FString Msg = FString::Printf(TEXT("Reloaded: %d | Current: %d | Remain: %d"), AmmoToReload, GetCurrentAmmo(), GetTotalAmmo());
	UKismetSystemLibrary::PrintString(this, Msg);

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
	case EWeaponType::Rifle:   return TEXT("Rifle");
	case EWeaponType::Pistol:  return TEXT("Pistol");
		// 필요에 따라 추가
	default:                   return TEXT("Unknown");
	}
}

void ATWeaponBase::ResetCanFire()
{
	bCanFire = true;
}