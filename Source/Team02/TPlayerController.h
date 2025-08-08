#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ATWeaponBase;
class UTUIManager;

UCLASS()
class TEAM02_API ATPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ATPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;


public:
	// 리스폰 전에 게임오버 UI 요청하는 코드
	UFUNCTION(BlueprintCallable, Category="Respawn")
	void RequestRespawnFromUI();
	
private:
	// UI 매니저 참조 관련
	UPROPERTY()
	TObjectPtr<UTUIManager> UIManager;

};
