#include "TPlayerUIWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UTPlayerUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 위젯 바인딩 상태 로그
	UE_LOG(LogTemp, Log, TEXT("[TPlayerUIWidget] NativeConstruct called"));
	UE_LOG(LogTemp, Log, TEXT("[TPlayerUIWidget] HPBar: %s"), HPBar ? TEXT("Valid") : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("[TPlayerUIWidget] HPText: %s"), HPText ? TEXT("Valid") : TEXT("NULL"));

	// Android에서 위젯 바인딩이 완전히 완료될 때까지 약간의 지연 후 초기화 플래그 설정
	if (UWorld* World = GetWorld())
	{
		FTimerHandle InitTimerHandle;
		World->GetTimerManager().SetTimer(
			InitTimerHandle,
			[this]()
			{
				bIsWidgetInitialized = true;
				UE_LOG(LogTemp, Log, TEXT("[TPlayerUIWidget] Widget fully initialized"));
			},
			0.1f,
			false
		);
	}
	else
	{
		// World가 없으면 즉시 초기화 (에디터 등)
		bIsWidgetInitialized = true;
	}
}

void UTPlayerUIWidget::UpdateHPBar(float CurrentHP, float MaxHP)
{
	// Android 초기화 타이밍 이슈 방지
	if (!bIsWidgetInitialized)
	{
		return;
	}

	if (!IsValid(HPBar) || !IsValid(HPText))
	{
		return;
	}

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

void UTPlayerUIWidget::UpdateAmmoInfo(int32 CurrentAmmo, int32 TotalAmmo)
{
	if (!bIsWidgetInitialized || !IsValid(AmmoText))
	{
		return;
	}

	if (AmmoText)
	{
		FString AmmoString;

		//total ammo가 매우 큰값이면 무한대표시
		if (TotalAmmo>=999)
		{
			AmmoString = FString::Printf(TEXT("%d/∞"), CurrentAmmo);
		}
		else
		{
			//일반적인 경우 (제한된 탄창)
			AmmoString=FString::Printf(TEXT("%d/%d"),CurrentAmmo,TotalAmmo);
		}

		AmmoText->SetText(FText::FromString(AmmoString));
		
	}
}

void UTPlayerUIWidget::ShowCaptureUI(const FString& AreaName)
{
	if (!bIsWidgetInitialized)
	{
		return;
	}

	// 거점 UI 요소들 표시
	if (IsValid(CaptureBar))
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
		
	}
	
}

void UTPlayerUIWidget::HideCaptureUI()
{
	if (!bIsWidgetInitialized)
	{
		return;
	}

	//거점 UI 요소들 숨김
	if (IsValid(CaptureBar))
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
	
}

void UTPlayerUIWidget::UpdateCaptureProgress(float Progress)
{
	if (!bIsWidgetInitialized)
	{
		return;
	}

	// 진행도 업데이트
	Progress=FMath::Clamp(Progress,0.0f,1.0f);

	if (IsValid(CaptureBar))
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
	if (!bIsWidgetInitialized || !IsValid(ObjectText))
	{
		return;
	}

	if (ObjectText)
	{
		FString CurrentText=ObjectText->GetText().ToString();
		
		bool bIsRealMissionChange=IsRealMissionChange(CurrentText,ObjectiveText);
		
		if (CurrentText.IsEmpty() || CurrentText==TEXT("Kill Monsters")|| CurrentText==TEXT("Mission:"))
		{
			//처음 미션이거나 "Mission:" 상테에서 변경(타이핑 효과)
			StartTypingAnimation(ObjectiveText);
			
		}
		else if (bIsRealMissionChange)
		{
			// 진짜 미션 변경: 깜빡임 + 타이핑
			StartFlashingAndChangeText(ObjectiveText);
		}
		else
		{
			// 숫자만 변경: 즉시 업데이트 (깜빡임 없음)
			ObjectText->SetText(FText::FromString(ObjectiveText));
		}
		
	}
}

void UTPlayerUIWidget::UpdateKillCount(int32 CurrentKills,int32 TotalMonsters)
{
	if (!bIsWidgetInitialized || !IsValid(KillCountText))
	{
		return;
	}

	if (KillCountText)
	{
		FString KillString=FString::Printf(TEXT("Monster: %d/%d"),CurrentKills,TotalMonsters);
		KillCountText->SetText(FText::FromString(KillString));
		
	}
}

void UTPlayerUIWidget::ShoWEnemyIncomingAlarm()
{
	if (!bIsWidgetInitialized || !IsValid(WaveAlarmText))
	{
		return;
	}

	if (WaveAlarmText)
	{
		WaveAlarmText->SetText(FText::FromString(TEXT("Enemy Incoming!!")));
		WaveAlarmText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UTPlayerUIWidget::HideEnemyIncomingAlarm()
{
	if (!bIsWidgetInitialized || !IsValid(WaveAlarmText))
	{
		return;
	}

	if (WaveAlarmText)
	{
		WaveAlarmText->SetVisibility(ESlateVisibility::Hidden);
		WaveAlarmText->SetText(FText::FromString(TEXT(""))); //문구까지 초기화
		
	}
}

void UTPlayerUIWidget::StartTypingAnimation(const FString& FullText)
{
	if (!bIsWidgetInitialized || !IsValid(ObjectText) || !GetWorld())
	{
		return;
	}

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
	
	
}

void UTPlayerUIWidget::UpdateTypingText()
{
	if (!bIsWidgetInitialized || !IsValid(ObjectText) || !GetWorld())
	{
		return;
	}

	if (!bIsTyping || CurrentCharIndex>=TargetText.Len())
	{
		//타이핑 완료
		bIsTyping=false;
		GetWorld()->GetTimerManager().ClearTimer(TypingTimerHandle);
		CurrentDisplayText=TargetText;
		ObjectText->SetText(FText::FromString(CurrentDisplayText));
		return;
	}
	// 한 글자씩 추가
	CurrentDisplayText=TargetText.Left(CurrentCharIndex+1);
	ObjectText->SetText(FText::FromString(CurrentDisplayText));
	CurrentCharIndex++;
}

void UTPlayerUIWidget::StartFlashingAndChangeText(const FString& NewText)
{
	if (!bIsWidgetInitialized || !IsValid(ObjectText) || !GetWorld())
	{
		return;
	}

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
	if (!bIsWidgetInitialized || !IsValid(ObjectText))
	{
		return;
	}

	static bool bVisible=false;
	bVisible=!bVisible;

	FLinearColor TextColor=bVisible ? FLinearColor::White : FLinearColor::Transparent;
	ObjectText->SetColorAndOpacity(FSlateColor(TextColor));
	
}

void UTPlayerUIWidget::StopFlashing()
{
	if (!GetWorld())
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(FlashTimerHandle);

	if (IsValid(ObjectText))
	{
		ObjectText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	}
	
}

bool UTPlayerUIWidget::IsRealMissionChange(const FString& OldText, const FString& NewText)
{

	if (OldText==TEXT("Mission:")&& NewText != TEXT("Mission:"))
	{
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
	return bIsRealChange;
	
}

void UTPlayerUIWidget::UpdateWeaponName(const FString& WeaponName)
{
	if (!bIsWidgetInitialized || !IsValid(WeaponNameText))
	{
		return;
	}

	if (WeaponNameText)
	{
		WeaponNameText->SetText(FText::FromString(WeaponName));
	}
	
}

void UTPlayerUIWidget::ShowHitMarker()
{
	if (!bIsWidgetInitialized || !IsValid(HitMarker) || !GetWorld())
	{
		return;
	}

	if (HitMarker)
	{
		// UImage로 캐스팅해서 함수 사용
		if (UImage* HitMarkerImage = Cast<UImage>(HitMarker))
		{
			
			GetWorld()->GetTimerManager().ClearTimer(HitMarkerTimerHandle);
			HitMarkerImage->SetVisibility(ESlateVisibility::Visible);
			HitMarkerImage->SetOpacity(1.0f);
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
            
			// ⏱️ 0.3초 후 히트마커 숨김
			GetWorld()->GetTimerManager().SetTimer(
				HitMarkerTimerHandle,
				[this, HitMarkerImage]()
				{
					if (HitMarkerImage && IsValid(HitMarkerImage))
					{
						HitMarkerImage->SetVisibility(ESlateVisibility::Hidden);
					}
				},
				0.3f,
				false
			);
			
		}
		
	}
}

void UTPlayerUIWidget::ShowDashReady()
{
	if (!bIsWidgetInitialized || !IsValid(DashText))
	{
		return;
	}

	if (DashText)
	{
		DashText->SetText(FText::FromString(TEXT("Dash Ready!")));
		DashText->SetVisibility(ESlateVisibility::Visible);
		DashText->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
		DashText->SetRenderScale(FVector2D(1.0f,1.0f));
	}
}




