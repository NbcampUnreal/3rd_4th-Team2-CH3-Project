#include "TPlayerUIWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UTPlayerUIWidget::UpdateHPBar(float CurrentHP, float MaxHP)
{
	if (HPBar)
	{
		float HPPercent=(MaxHP>0.0f) ? (CurrentHP/MaxHP) : 0.0f;
		HPBar->SetPercent(HPPercent);
	}
	if (HPText)
	{
		float HPPercent=(MaxHP>0.0f) ? (CurrentHP/ MaxHP) : 0.0f;
		FString HPString=FString::Printf(TEXT("%.0f%%"),HPPercent * 100.0f);
		HPText->SetText(FText::FromString(HPString));
	}
}

void UTPlayerUIWidget::UpdateAmmoInfo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (AmmoText)
	{
		FString AmmoString=FString::Printf(TEXT("%d/%d"),CurrentAmmo,MaxAmmo);
		AmmoText->SetText(FText::FromString(AmmoString));
	}
}

void UTPlayerUIWidget::UpdateWaveTime(const FString& TimeString)
{
	if (WaveTimeText)
	{
		WaveTimeText->SetText(FText::FromString(TimeString));
	}
}

