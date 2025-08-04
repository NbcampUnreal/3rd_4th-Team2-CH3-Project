#pragma once

#include "CoreMinimal.h"
#include "Item/TItemBase.h"
#include "Bullet/TBullet.h"
#include "TWeaponBase.generated.h"
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Shotgun,
	Rifle,
	Pistol,
};
class ATItemBase;

UCLASS()
class TEAM02_API ATWeaponBase : public ATItemBase
{
	GENERATED_BODY()
protected:
	int32 TotalAmmo = 180;
	int32 CurrentAmmo = 30;   // 탄창에 남은 탄약 (기본값)
public:
	ATWeaponBase();

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MuzzlPoint")
	USceneComponent* MuzzlePoint;

	// --- 무기 속성 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Damage;

	UFUNCTION(BlueprintCallable, Category="Weapon")
	int32 GetTotalAmmo() const { return TotalAmmo; }

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void SetTotalAmmo(int32 NewAmmo) { TotalAmmo = FMath::Clamp(NewAmmo, 0, MaxTotalAmmo); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	int32 MaxTotalAmmo = 180;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 MaxAmmo;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetCurrentAmmo(int32 NewAmmo)
	{
		CurrentAmmo = FMath::Clamp(NewAmmo, 0, MaxAmmo);
		// (선택) 여기서 UI 업데이트, 사운드, 로그 등 추가 가능
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireRate;        // 연사 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float ReloadTime;      // 장전 시간

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Range;           // 사거리

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ATBullet> BulletClass;

	// --- 무기 동작 ---
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Reload();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual bool CanFire() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual bool CanReload() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Equip();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Unequip();

	UFUNCTION(BlueprintCallable, Category="Weapon")
	FString GetWeaponTypeString() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWeaponType WeaponType= EWeaponType::Rifle;
	
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	virtual void ResetCanFire();

	
protected:
	FTimerHandle FireRateTimerHandle;
	bool bCanFire = true;
	
	FTimerHandle ReloadTimerHandle;
};
