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

	// 직접 몬스터 이름을 설정하는 함수
	UFUNCTION(BlueprintCallable)
	void SetMonsterName(const FString& NewName);

	// show or hide UI wigets
	UFUNCTION(BlueprintCallable)
	void ShowHealthBar();

	UFUNCTION(BlueprintCallable)
	void HideHealthBar();

	//몬스터 타입에 따라 이름 설정하는 함수
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
