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

	virtual void Tick(float DeltaSeconds) override;

	virtual void HandleOnPostCharacterDead() override;

	UFUNCTION()
	void RequestRespawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;


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

	void InputStartZoom(const FInputActionValue& InputActionValue);

	void InputEndZoom(const FInputActionValue& InputActionValue);

	void OnPause(const FInputActionValue& InValue);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UTInputConfig> PlayerCharacterInputConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UInputMappingContext> PlayerCharacterInputMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<ATWeaponBase> DefaultWeaponClass;

#pragma endregion

#pragma region Zoom
	
protected:
	float TargetFOV = 70.f;

	float CurrentFOV = 70.f;

	// 위젯 클래스 선언	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PauseMenu")
	TSubclassOf<UUserWidget> PauseMenuClass;

	// 위젯 인스턴스 캐싱용
	UUserWidget* PauseMenuInstance;

	UFUNCTION(BlueprintCallable, Category = "PauseMenu")
	void ResumeGame();

#pragma endregion
};
