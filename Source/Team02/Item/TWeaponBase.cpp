// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TWeaponBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/TPlayerCharacter.h"
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
		// 1. 총구 위치/회전 구하기
		FVector MuzzleLoc = MuzzlePoint->GetComponentLocation();
		FRotator MuzzleRot = MuzzlePoint->GetComponentRotation();

		// 2. Spawn 파라미터 준비
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();

		// 3. 총알(Projectile) 스폰!
		ATBullet* Bullet = GetWorld()->SpawnActor<ATBullet>(
		BulletClass,
		MuzzleLoc,
		MuzzleRot,
		SpawnParams
		);
		if (Bullet)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("총알 생성 OK!"));
			Bullet->SetDamage(Damage);
			Bullet->SetRange(Range);
			// 기타 세팅
		}

		bCanFire = false;
		GetWorld()->GetTimerManager().SetTimer(
			FireRateTimerHandle,
			this, &ATWeaponBase::ResetCanFire,
			FireRate, false
		);
		UKismetSystemLibrary::PrintString(this, TEXT("Fire() called!"));
		
		// TODO: 투사체 발사 로직
		SetCurrentAmmo(GetCurrentAmmo() - 1);
	}
	else if (!bCanFire)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Fire(): FireRate!!"));
	}
	else
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Fire(): No ammo!"));
	}
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