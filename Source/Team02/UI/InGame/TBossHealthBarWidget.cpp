#include "TBossHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UTBossHealthBarWidget::UpdateHealthBar(float CurrentHP, float MaxHP)
{
	if (BossHealthBar && HealthText)
	{
		//caculate hp percent
		float HealthPercent=(MaxHP>0.0f) ? (CurrentHP/MaxHP) : 0.0f;

		// progress bar update
		BossHealthBar->SetPercent(HealthPercent);

		// Hp text update( use percent)
		FString HealthString=FString::Printf(TEXT("%.0f%%"),HealthPercent * 100.0f);
		HealthText->SetText(FText::FromString(HealthString));
		
	}
}

void UTBossHealthBarWidget::SetBossName(const FString& Name)
{
	if (BossName)
	{
		BossName->SetText(FText::FromString(Name));
	}
}

void UTBossHealthBarWidget::ShowHealthBar()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UTBossHealthBarWidget::HideHealthBar()
{
	SetVisibility(ESlateVisibility::Hidden);
}

