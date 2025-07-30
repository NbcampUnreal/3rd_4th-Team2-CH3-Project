#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ATWeaponBase;
UCLASS()
class TEAM02_API ATPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ATPlayerController();
	
	// UPROPERTY()
	// ATWeaponBase* CurrentWeapon;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	// TSubclassOf<ATWeaponBase> DefaultWeaponClass;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	// UInputMappingContext* SpectatorMappingContext;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	// UInputAction* FireAction;
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	// UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> PlayerUIWidgetClass;
	

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// // 입력 콜백
	// UFUNCTION()
	// void OnFire();
	// UFUNCTION()
	// void OnReload();
};
