#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TMonsterHealthBarWidget.generated.h"

UCLASS()
class TEAM02_API UTMonsterHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// healthbar update function
	UFUNCTION(BlueprintCallable)
	void UpdateHealthBar(float CurrentHP,float MaxHP);

	// monster name function
	UFUNCTION(BlueprintCallable)
	void SetMonsterName(const FString& Name);

	// show or hide UI wigets
	UFUNCTION(BlueprintCallable)
	void ShowHealthBar();

	UFUNCTION(BlueprintCallable)
	void HideHealthBar();

protected:
	// widget binding
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MonsterNameText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> HealthText;
};
