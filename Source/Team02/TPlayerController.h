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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> PlayerUIWidgetClass;
	
	UPROPERTY()
	class UUserWidget* PlayerUIWidget;

	UFUNCTION(BlueprintCallable,Category="UI")
	void UpdateHPBar();

	UFUNCTION()
	void OnGameTimeUpdate(float NewTime);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	FTimerHandle UIUpdateTimerHandle;
};
