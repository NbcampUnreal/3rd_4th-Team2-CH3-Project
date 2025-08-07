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
class UAnimMontage;
class UNiagaraSystem;
class USoundBase;

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

	// 총 총알 최대치 가져오기
	UFUNCTION(BlueprintCallable, Category="Weapon")
	int32 GetTotalAmmo() const { return TotalAmmo; }

	// 총 총알 최대치 세팅
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void SetTotalAmmo(int32 NewAmmo) { TotalAmmo = FMath::Clamp(NewAmmo, 0, MaxTotalAmmo); }

	// 총 총알의 최대치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	int32 MaxTotalAmmo = 180;

	//장전할 수 있는 총알의 최대치
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
	virtual void FireFrom(FVector Start, FVector FireDir);
	
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

	UAnimMontage* GetAttackMontage();
	
	// === 파티클/사운드 ===
	UFUNCTION()
	void FireSounds(FVector& MuzzleLoc);
	UFUNCTION()
	void FireEffect(FVector& MuzzleLoc, FVector& MuzzleRot) const;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX")
	UNiagaraSystem* MuzzleFlashFX; // 총구 이펙트

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX")
	USoundBase* FireSound;
protected:
	FTimerHandle FireRateTimerHandle;
	bool bCanFire = true;
	
	FTimerHandle ReloadTimerHandle;

#pragma region Montage

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage;

#pragma endregion
};
