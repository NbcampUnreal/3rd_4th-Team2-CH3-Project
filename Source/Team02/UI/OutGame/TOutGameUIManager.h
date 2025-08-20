// TOutGameUIManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "TOutGameUIManager.generated.h"

UCLASS()
class TEAM02_API AOutGameUIManager : public AActor
{
	GENERATED_BODY()
	
public:
	AOutGameUIManager();

	// Restart 시 호출 (이벤트 해제 → UI 제거 → 이벤트 재구독)
	UFUNCTION(BlueprintCallable, Category = "Game")
	void HandleRestart();
	
	// Quit 버튼용 함수 선언 추가
	UFUNCTION(BlueprintCallable, Category = "UI")
	void QuitToMainMenu();
	
protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 승리 화면 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	// 게임 오버 화면 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	// 생성된 위젯 인스턴스들
	UPROPERTY()
	TObjectPtr<UUserWidget> VictoryWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> GameOverWidget;

	// UIManager 이벤트 수신 함수
	UFUNCTION()
	void ShowVictoryScreen();

	UFUNCTION()
	void ShowGameOverScreen();
	
private:
	// UIManager 델리게이트 구독 (중복 방지 처리 포함)
	void SubscribeToUIManagerEvents();

	// UIManager 델리게이트 구독 해제
	void UnsubscribeFromUIManagerEvents();

	// UI와 게임 상태 초기화 (Restart 대비)
	void ResetUIAndState();
	
};
