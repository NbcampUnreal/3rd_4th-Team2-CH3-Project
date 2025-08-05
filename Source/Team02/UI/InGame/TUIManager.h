#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TPlayerUIWidget.h"
#include "TUIManager.generated.h"

class ATCharacterBase;
class ATWeaponBase;

UCLASS()
class TEAM02_API UTUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// initializce Subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//create UI
	UFUNCTION(BlueprintCallable)
	void CreatePlayerUI();

	// update ui functions
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerHP();
	
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerAmmo();

	UFUNCTION(BlueprintCallable)
	void UpdateWaveInfo(const FString& TimeString,int32 InCurrentWave,int32 InMaxWave);

	// Player references
	UFUNCTION(BlueprintCallable)
	void SetPlayerCharacter(ATCharacterBase* PlayerChar);

protected:
	// UI widget class
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UTPlayerUIWidget> PlayerUIWidgetClass;

	//UI widget instance
	UPROPERTY()
	TObjectPtr<UTPlayerUIWidget> PlayerUIWidget;

	//Player reference
	UPROPERTY()
	TObjectPtr<ATCharacterBase> PlayerCharacter;

	// current weapon reference
	UPROPERTY()
	TObjectPtr<ATWeaponBase> CurrentWeapon;

	// Timer
	FTimerHandle UIUpdateTimerHandle;

	// Wave Functions
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float WaveTime=300.0f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 CurrentWave=1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 MaxWave=2;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	bool bWaveActive=false;

	//Wave Timer
	FTimerHandle WaveTimerHandle;

	// update Wavetime
	void UpdateWaveTime();

	//시간 분초 포맷
	FString GetFormattedTime() const;
	
	// regularly ui update
	void UpdateAllUI();
	
	
};
