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
	// 체력, 탄창
	UFUNCTION(BlueprintCallable)
	void UpdateHPBar(float CurrentHP, float MaxHP);
    
	UFUNCTION(BlueprintCallable)
	void UpdateAmmoInfo(int32 CurrentAmmo, int32 MaxAmmo);
	
	//거점 점령 관련 함수
	UFUNCTION(BlueprintCallable)
	void ShowCaptureUI(const FString& AreaName);
	
	UFUNCTION(BlueprintCallable)
	void HideCaptureUI();
	
	UFUNCTION(BlueprintCallable)
	void UpdateCaptureProgress(float Progress);

	//거점 UI 표시 상태 확인 함수 추가
	UFUNCTION(BlueprintCallable)
	bool IsCaptureUIVisible() const;

	// 임무 관련 함수 추가
	UFUNCTION(BlueprintCallable)
	void UpdateMissionObjective(const FString& ObjectiveText);

	// 몬스터 처치 관련
	UFUNCTION(BlueprintCallable)
	void UpdateKillCount(int32 CurrentKills,int32 TotalMonsters);

	//적 스폰 알림 함수 추가
	UFUNCTION(BlueprintCallable)
	void ShoWEnemyIncomingAlarm();

	UFUNCTION(BlueprintCallable)
	void HideEnemyIncomingAlarm();

	//타이핑 애니메이션 함수
	UFUNCTION(BLueprintCallable)
	void StartTypingAnimation(const FString& FullText);

	UFUNCTION(BlueprintCallable)
	void StartFlashingAndChangeText(const FString& NewText);
	


protected:
	// ⭐ 각 이름이 한 번씩만 나와야 함
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HPText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WaveAlarmText;

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

	//타이밍 애니메이션 변수
	UPROPERTY()
	FString TargetText; // 최종 표시 텍스트

	UPROPERTY()
	FString CurrentDisplayText; // 현재 표시중인 텍스트

	UPROPERTY()
	int32 CurrentCharIndex; // 현재 글자 인덱스

	UPROPERTY()
	bool bIsTyping; // 타이핑 중인지 확인

	FTimerHandle TypingTimerHandle; // 타이핑 타이머
	FTimerHandle FlashTimerHandle; 

private:
	// 타이핑 애니메이션 함수
	void UpdateTypingText();
	void FlashText();
	void StopFlashing();

	// 진짜 미션인지 확인 하는 함수
	bool IsRealMissionChange(const FString& OldText,const FString& NewText);
	
};