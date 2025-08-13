#include "TPlayerUIWidget.h"

#include "Character/TNonPlayerCharacter.h"
#include "Character/TNonPlayerCharacterSword.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "DSP/DelayStereo.h"

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
		// 현재 텍스트가 비어있으면 처음 미션
		FString CurrentText=ObjectText->GetText().ToString();

		// 진짜 미션 변경 인지 확인
		bool bIsRealMissionChange=IsRealMissionChange(CurrentText,ObjectiveText);

		// 조건 수정: "Mission:" 이상테에서 실제 미션으로 변경
		if (CurrentText.IsEmpty() || CurrentText==TEXT("Kill Monsters")|| CurrentText==TEXT("Mission:"))
		{
			//처음 미션이거나 "Mission:" 상테에서 변경(타이핑 효과)
			StartTypingAnimation(ObjectiveText);
			UE_LOG(LogTemp,Warning,TEXT("Initial mission typing: %s"),*ObjectiveText);
		}
		else if (bIsRealMissionChange)
		{
			// 진짜 미션 변경: 깜빡임 + 타이핑
			StartFlashingAndChangeText(ObjectiveText);
			UE_LOG(LogTemp, Warning, TEXT("Real mission change with effects: %s"), *ObjectiveText);
		}
		else
		{
			// 숫자만 변경: 즉시 업데이트 (깜빡임 없음)
			ObjectText->SetText(FText::FromString(ObjectiveText));
			UE_LOG(LogTemp, Warning, TEXT("Number only update: %s"), *ObjectiveText);
		}
		
	}
}

void UTPlayerUIWidget::UpdateKillCount(int32 CurrentKills,int32 TotalMonsters)
{
	if (KillCountText)
	{
		FString KillString=FString::Printf(TEXT("Monster: %d/%d"),CurrentKills,TotalMonsters);
		KillCountText->SetText(FText::FromString(KillString));

		UE_LOG(LogTemp,Warning,TEXT("Kill count updated: %s"), *KillString);
	}
}

void UTPlayerUIWidget::ShoWEnemyIncomingAlarm()
{
	if (WaveAlarmText)
	{
		WaveAlarmText->SetText(FText::FromString(TEXT("Enemy Incoming!!")));
		WaveAlarmText->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp,Warning,TEXT("Enemy Incoming alram shown!!"));
	}
}

void UTPlayerUIWidget::HideEnemyIncomingAlarm()
{
	if (WaveAlarmText)
	{
		WaveAlarmText->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp,Warning,TEXT("Enemy Incoming alarm hidden!!"));
	}
}

void UTPlayerUIWidget::StartTypingAnimation(const FString& FullText)
{
	if (!ObjectText) return;

	// 기존 타이핑 중이면 중단
	GetWorld()->GetTimerManager().ClearTimer(TypingTimerHandle);

	TargetText=FullText;
	CurrentDisplayText=TEXT("");
	CurrentCharIndex=0;
	bIsTyping=true;
	
	//0.05초마다 글자 하나씩 추가
	GetWorld()->GetTimerManager().SetTimer(
		TypingTimerHandle,
		this,
		&UTPlayerUIWidget::UpdateTypingText,
		0.05f,
		true
		);

	UE_LOG(LogTemp,Warning,TEXT("Started typing animation for: %s"), *FullText);
	
}

void UTPlayerUIWidget::UpdateTypingText()
{
	if (!bIsTyping || CurrentCharIndex>=TargetText.Len())
	{
		//타이핑 완료
		bIsTyping=false;
		GetWorld()->GetTimerManager().ClearTimer(TypingTimerHandle);
		CurrentDisplayText=TargetText;
		ObjectText->SetText(FText::FromString(CurrentDisplayText));
		UE_LOG(LogTemp,Warning,TEXT("Typing animation completed"));
		return;
	}
	// 한 글자씩 추가
	CurrentDisplayText=TargetText.Left(CurrentCharIndex+1);
	ObjectText->SetText(FText::FromString(CurrentDisplayText));
	CurrentCharIndex++;
}

void UTPlayerUIWidget::StartFlashingAndChangeText(const FString& NewText)
{
	if (!ObjectText) return;

	UE_LOG(LogTemp,Warning,TEXT("Starting flash effect for mission change"));

	// 깜빡임 시작
	GetWorld()->GetTimerManager().SetTimer(
		FlashTimerHandle,
		this,
		&UTPlayerUIWidget::FlashText,
		0.1f,
		true
		);

	// 1초후 깜빡임 중단하고 새 텍스트 타이핑 시작
	FTimerHandle StopFlashTimer;
	GetWorld()->GetTimerManager().SetTimer(
		StopFlashTimer,
		[this,NewText]()
		{
			StopFlashing();
			StartTypingAnimation(NewText);
		},
		1.0f,
		false
		);
}

void UTPlayerUIWidget::FlashText()
{
	if (!ObjectText) return;

	static bool bVisible=false;
	bVisible=!bVisible;

	FLinearColor TextColor=bVisible ? FLinearColor::White : FLinearColor::Transparent;
	ObjectText->SetColorAndOpacity(FSlateColor(TextColor));

	
}

void UTPlayerUIWidget::StopFlashing()
{
	GetWorld()->GetTimerManager().ClearTimer(FlashTimerHandle);

	if (ObjectText)
	{
		ObjectText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	}

	UE_LOG(LogTemp,Warning,TEXT("Flash effect stopped"));
}

bool UTPlayerUIWidget::IsRealMissionChange(const FString& OldText, const FString& NewText)
{

	if (OldText==TEXT("Mission:")&& NewText != TEXT("Mission:"))
	{
		UE_LOG(LogTemp, Warning, TEXT("Mission initialization: 'Mission:' -> '%s' = REAL CHANGE"), *NewText);
		return true;
	}

	
	// 진짜 미션 변경 패턴
	TArray<FString> MissionTypes={
	TEXT("Eliminate enemies"),
	TEXT("Move to control point"),
	TEXT("Capture the control"),
	TEXT("Victory")
	};

	// 이전 텍스트와 새 텍스트 유형 찾기
	FString OldType=TEXT("");
	FString NewType=TEXT("");

	for (const FString& Type : MissionTypes)
	{
		if (OldText.Contains(Type))
		{
			OldType=Type;
		}
		if (NewText.Contains(Type))
		{
			NewType=Type;
		}
	}

	// 미션 유형이 다르면 진짜 변경
	bool bIsRealChange=(OldType != NewType) && !NewType.IsEmpty();

	UE_LOG(LogTemp, Warning, TEXT("Mission change check: '%s' -> '%s' = %s"), 
		  *OldType, *NewType, bIsRealChange ? TEXT("REAL CHANGE") : TEXT("NUMBER UPDATE"));

	return bIsRealChange;
	
}

void UTPlayerUIWidget::UpdateWeaponName(const FString& WeaponName)
{
	if (WeaponNameText)
	{
		WeaponNameText->SetText(FText::FromString(WeaponName));
		UE_LOG(LogTemp,Warning,TEXT("Weapon name updated: %s"),*WeaponName);
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("WeaponNameText is NULL! Check widget binding,"));
	}
	
}

void UTPlayerUIWidget::ShowHitMarker()
{
	if (HitMarker)
	{
		// 🔧 UImage로 캐스팅해서 함수 사용
		if (UImage* HitMarkerImage = Cast<UImage>(HitMarker))
		{
			// 기존 타이머 정리
			GetWorld()->GetTimerManager().ClearTimer(HitMarkerTimerHandle);
            
			// 🎯 히트마커 표시
			HitMarkerImage->SetVisibility(ESlateVisibility::Visible);
			HitMarkerImage->SetOpacity(1.0f);
            
			// 📏 임팩트 효과: 크기 변화
			HitMarkerImage->SetRenderScale(FVector2D(1.8f, 1.8f)); // 1.8배로 크게 시작
            
			// ⚡ 0.1초 후 원래 크기로
			FTimerHandle ScaleTimer;
			GetWorld()->GetTimerManager().SetTimer(
				ScaleTimer,
				[this, HitMarkerImage]()
				{
					if (HitMarkerImage && IsValid(HitMarkerImage))
					{
						HitMarkerImage->SetRenderScale(FVector2D(1.0f, 1.0f));
					}
				},
				0.1f,
				false
			);
            
			// ⏱️ 0.15초 후 히트마커 숨김
			GetWorld()->GetTimerManager().SetTimer(
				HitMarkerTimerHandle,
				[this, HitMarkerImage]()
				{
					if (HitMarkerImage && IsValid(HitMarkerImage))
					{
						HitMarkerImage->SetVisibility(ESlateVisibility::Hidden);
						UE_LOG(LogTemp, Warning, TEXT("🎯 Red hitmarker hidden"));
					}
				},
				0.15f,
				false
			);
            
			UE_LOG(LogTemp, Warning, TEXT("🔴 RED HITMARKER shown! Scale: 1.8x -> 1.0x"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ HitMarker is not UImage! Check widget type."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ HitMarker widget not found! Check Blueprint binding."));
	}
}




