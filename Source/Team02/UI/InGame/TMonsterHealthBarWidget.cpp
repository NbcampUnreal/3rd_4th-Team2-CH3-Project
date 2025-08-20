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
		
		// hp text update (ex: 70%/100%)
		FString HealthString=FString::Printf(TEXT("%.0f%%"),HealthPercent * 100.0f);
		HealthText->SetText(FText::FromString(HealthString));
	}
}

void UTMonsterHealthBarWidget::SetMonsterNameByType(AActor* OwnerMonster)
{
	if (!OwnerMonster || !MonsterNameText)
	{
		return;
	}

	FString ClassName=OwnerMonster->GetClass()->GetName();
	FString MonsterTypeName;
	
	// 클래스 이름으로 몬스터 타입 구분
	if (ClassName.Contains(TEXT("GunNPC")) || ClassName.Contains(TEXT("Gun")))
	{
		MonsterTypeName = TEXT("Gun Enemy");
	}
	else if (ClassName.Contains(TEXT("SwordNPC")) || ClassName.Contains(TEXT("Sword")))
	{
		MonsterTypeName = TEXT("Sword Enemy");
	}
	else
	{
		MonsterTypeName = TEXT("Enemy");
	}
	
	MonsterNameText->SetText(FText::FromString(MonsterTypeName));
}


void UTMonsterHealthBarWidget::SetMonsterName(const FString& NewName)
{
	if (MonsterNameText)
	{
		MonsterNameText->SetText(FText::FromString(NewName));
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