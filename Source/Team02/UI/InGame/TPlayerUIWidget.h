#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TPlayerUIWidget.generated.h"

UCLASS()
class TEAM02_API UTPlayerUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateHPBar(float CurrentHP, float MaxHP);
    
	UFUNCTION(BlueprintCallable)
	void UpdateAmmoInfo(int32 CurrentAmmo, int32 MaxAmmo);
    
	UFUNCTION(BlueprintCallable)
	void UpdateWaveTime(const FString& TimeString);

protected:
	// ⭐ 각 이름이 한 번씩만 나와야 함
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HPText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WaveTimeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WaveComeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ObjectText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> KillCountText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> CaptureBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CapturePercent;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CaptureLabel;
};