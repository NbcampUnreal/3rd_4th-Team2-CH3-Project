#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ATWeaponBase;
class UUserWidget;
UCLASS()
class TEAM02_API ATPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ATPlayerController();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> PlayerUIWidgetClass;

	// UI 위젯 참조 지정
	UPROPERTY()
	class UUserWidget* PlayerUIWidget;

	// HP바 업데이트 함수
	UFUNCTION(BlueprintCallable,Category="UI")
	void UpdateHPBar();

	// 웨이브 델리게이트 관련함수
	UFUNCTION()
	void OnGameTimeUpdate(float NewTime);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	//UI 업데이트용 타이머
	FTimerHandle UIUpdateTimerHandle;
};
