#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "TitleUIPlayerController.generated.h"


UCLASS()
class TEAM02_API ATitleUIPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// 레벨 시작 시 입력 모드 및 UI 초기화
	virtual void BeginPlay() override;

public:
	// 메뉴 UI 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	// 현재 표시 중인 메뉴 UI 인스턴스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	UUserWidget* MainMenuWidgetInstance;

	// 메인 메뉴 표시 함수
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowMainMenu(bool bIsRestart);

	// 게임 시작
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();

};
