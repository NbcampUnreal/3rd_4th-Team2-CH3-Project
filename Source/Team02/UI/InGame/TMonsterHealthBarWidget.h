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
	
	UFUNCTION(BlueprintCallable)
	void UpdateHealthBar(float CurrentHP,float MaxHP);
	
	UFUNCTION(BlueprintCallable)
	void SetMonsterName(const FString& NewName);
	
	UFUNCTION(BlueprintCallable)
	void ShowHealthBar();

	UFUNCTION(BlueprintCallable)
	void HideHealthBar();
	
	UFUNCTION(BLueprintCallable)
	void SetMonsterNameByType(AActor* OwnerMonster);


protected:
	// widget binding
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MonsterNameText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> HealthText;
};
