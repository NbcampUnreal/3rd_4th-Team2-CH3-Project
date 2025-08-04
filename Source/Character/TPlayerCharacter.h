#pragma once

#include "CoreMinimal.h"
#include "Character/TCharacterBase.h"
#include "InputActionValue.h"
#include "TPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UTInputConfig;
class UInputMappingContext;
class ATWeaponBase;

UCLASS()
class TEAM02_API ATPlayerCharacter : public ATCharacterBase
{
	GENERATED_BODY()
        
#pragma region Override ACharacter
        
public:
	ATPlayerCharacter();
	
	virtual void BeginPlay() override;
	
	ATWeaponBase* CurrentWeapon;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UCameraComponent> CameraComponent;

#pragma endregion

#pragma region Input

private:
	void InputMove(const FInputActionValue& InValue);

	void InputLook(const FInputActionValue& InValue);
	
	void OnFire(const FInputActionValue& InValue);

	void OnReload(const FInputActionValue& InValue);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UTInputConfig> PlayerCharacterInputConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UInputMappingContext> PlayerCharacterInputMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<ATWeaponBase> DefaultWeaponClass;

#pragma endregion

};
