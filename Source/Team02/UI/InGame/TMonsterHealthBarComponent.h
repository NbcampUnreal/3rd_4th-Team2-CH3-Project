#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"
#include "TMonsterHealthBarComponent.generated.h"

class UTMonsterHealthBarWidget;
class ATCharacterBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM02_API UTMonsterHealthBarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTMonsterHealthBarComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 체력바 초기화
	UFUNCTION(BlueprintCallable)
	void InitializeHealthBar();

	//위잿 생성시 자동으로 몬스터 이름 설정
	UFUNCTION(BlueprintCallable)
	void InitializeMonsterName();

protected:
	// widget component(world space UI)
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBarWidgetComponent;

	//widget class reference
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UTMonsterHealthBarWidget> HealthBarWidgetClass;

	// 체력값
	float LastKnownCurrentHP;
	float LastKnownMaxHP;

	// 오너 캐릭터 레퍼런스
	UPROPERTY()
	TObjectPtr<ATCharacterBase> OwnerCharacter;

private:
	//내부적으로 사용하는 헬퍼함수
	void SetupMonsterNameAfterWidgetCreation();
		
};
