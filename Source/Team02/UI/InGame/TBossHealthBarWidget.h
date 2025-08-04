#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TBossHealthBarWidget.generated.h"


UCLASS()
class TEAM02_API UTBossHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// set hp update function
	UFUNCTION(BlueprintCallable)
	void UpdateHealthBar(float CurrentHP,float MaxHP);

	//set boss name
	UFUNCTION(BlueprintCallable)
	void SetBossName(const FString& Name);

	//show or hide widget
	UFUNCTION(BlueprintCallable)
	void ShowHealthBar();
	UFUNCTION(BlueprintCallable)
	void HideHealthBar();

protected:
	// widget binding

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> BossHealthBar;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> BossName;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> HealthText;



	
};
