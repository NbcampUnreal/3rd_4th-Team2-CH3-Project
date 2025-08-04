#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "TBossHealthBarComponent.generated.h"

class UTBossHealthBarWidget;
class ATCharacterBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM02_API UTBossHealthBarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTBossHealthBarComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// health bar initialize
	UFUNCTION(BlueprintCallable)
	void InitializeBossHealthBar();

protected:
	//widget component (world space ui)
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> BossHealthBarWidgetComponent;

	// widget class reference
	UPROPERTY(EditAnywhere,BlueprintReadonly)
	TSubclassOf<UTBossHealthBarWidget> BossHealthBarWidgetClass;

	//체력값 캐시
	float LastKnownCurrentHP;
	float LastKnownMaxHP;

	//owner 캐릭터 참조
	UPROPERTY()
	TObjectPtr<ATCharacterBase> OwnerCharacter;
	
};
