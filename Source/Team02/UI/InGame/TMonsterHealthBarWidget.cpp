#include "TMonsterHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UTMonsterHealthBarWidget::UpdateHealthBar(float CurrentHP,float MaxHP)
{
	if (HealthProgressBar && HealthText)
	{
		// calculate hp percent
		float HealthPercent=(MaxHP>0.0f) ? (CurrentHP/MaxHP) : 0.0f;
		
		// progress bar update
		HealthProgressBar->SetPercent(HealthPercent);
		
		// hp text update (ex: 70/100)
		FString HealthString=FString::Printf(TEXT("%.0f/%.0f"),CurrentHP,MaxHP);
		HealthText->SetText(FText::FromString(HealthString));
	}
}

void UTMonsterHealthBarWidget::SetMonsterName(const FString& Name)
{
	if (MonsterNameText)
	{
		MonsterNameText->SetText(FText::FromString(Name));
	}
}

void UTMonsterHealthBarWidget::ShowHealthBar()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UTMonsterHealthBarWidget::HideHealthBar()
{
	SetVisibility(ESlateVisibility::Hidden);
}