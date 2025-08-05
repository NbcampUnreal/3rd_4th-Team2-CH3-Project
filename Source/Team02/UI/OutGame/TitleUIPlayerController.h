// TitleUIPlayerController.h

#pragma once

#include "Blueprint/UserWidget.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TitleUIPlayerController.generated.h"

UCLASS()
class TEAM02_API ATitleUIPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;


public:

	ATitleUIPlayerController();
	
	// 메뉴 UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	UUserWidget* MainMenuWidgetInstance;

	// 메인 메뉴 표시
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowMainMenu(bool bIsRestart);

	// // 게임 시작
	// UFUNCTION(BlueprintCallable, Category = "Menu")
	// void StartGame();
	//
};
