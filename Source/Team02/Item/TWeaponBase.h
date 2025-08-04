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
public:
	ATWeaponBase();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MuzzlPoint")
	USceneComponent* MuzzlePoint;

	// --- 무기 속성 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 CurrentAmmo;

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
