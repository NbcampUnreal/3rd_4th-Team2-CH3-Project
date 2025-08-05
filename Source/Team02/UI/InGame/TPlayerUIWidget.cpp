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

void UTPlayerUIWidget::ShowCaptureUI(const FString& AreaName)
{
	// 거점 UI 요소들 표시
	if (CaptureBar)
	{
		CaptureBar->SetVisibility(ESlateVisibility::Visible);
		CaptureBar->SetPercent(0.0f); // 초기 진행도 0%
	}

	if (CapturePercent)
	{
		CapturePercent->SetVisibility(ESlateVisibility::Visible);
		CapturePercent->SetText(FText::FromString(TEXT("0%")));
	}

	if (CaptureLabel)
	{
		CaptureLabel->SetVisibility(ESlateVisibility::Visible);
		
		// 메시지를 깔끔하게 표시
		FString CaptureMessage=TEXT("Capturing area...");
		CaptureLabel->SetText(FText::FromString(CaptureMessage));

		//테스트 로그
		UE_LOG(LogTemp,Warning,TEXT("CaptureLabel set to Visible!!"));
	}
	
	
}

void UTPlayerUIWidget::HideCaptureUI()
{
	//거점 UI 요소들 숨김
	if (CaptureBar)
	{
		CaptureBar->SetVisibility(ESlateVisibility::Hidden);
	}

	if (CapturePercent)
	{
		CapturePercent->SetVisibility(ESlateVisibility::Hidden);
	}

	if (CaptureLabel)
	{
		CaptureLabel->SetVisibility(ESlateVisibility::Hidden);
	}

	//테스트 로그
	UE_LOG(LogTemp,Warning,TEXT("Capture UI Hidden"));
}

void UTPlayerUIWidget::UpdateCaptureProgress(float Progress)
{
	// 진행도 업데이트
	Progress=FMath::Clamp(Progress,0.0f,1.0f);

	if (CaptureBar)
	{
		CaptureBar->SetPercent(Progress);
	}

	if (CapturePercent)
	{
		//퍼센트로 표시
		FString ProgressString=FString::Printf(TEXT("%.0f%%"),Progress * 100.0f);
		CapturePercent->SetText(FText::FromString(ProgressString));
	}
	// 점령 완료시 메시지 변경
	if (CaptureLabel && Progress>=1.0f)
	{
		CaptureLabel->SetText(FText::FromString(TEXT("Captured!")));
	}
}

bool UTPlayerUIWidget::IsCaptureUIVisible() const
{
	if (CaptureBar)
	{
		return CaptureBar->GetVisibility()==ESlateVisibility::Visible;
	}
	return false;
}

void UTPlayerUIWidget::UpdateMissionObjective(const FString& ObjectiveText)
{
	if (ObjectText)
	{
		ObjectText->SetText(FText::FromString(ObjectiveText));
		UE_LOG(LogTemp,Warning,TEXT("Mission updated: %s"), *ObjectiveText);
	}
}