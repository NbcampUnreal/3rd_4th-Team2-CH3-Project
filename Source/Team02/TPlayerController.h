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

private:
	// UI 매니저 참조 관련
	UPROPERTY()
	TObjectPtr<UTUIManager> UIManager;
};
