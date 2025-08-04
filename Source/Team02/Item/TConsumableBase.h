// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TItemBase.h"
#include "TConsumableBase.generated.h"

UENUM(BlueprintType)
enum class EConsumableType : uint8
{
	Potion,           // 체력 회복
	Ammo 
};

UCLASS()
class TEAM02_API ATConsumableBase : public ATItemBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATConsumableBase();

	
	// 소모품 타입 (치유, 버프 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Consumable")
	EConsumableType ConsumableType = EConsumableType::Potion;

	// 효과량 (예: 체력/스태미나 회복량)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Consumable")
	float Amount = 50.0f;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item|Consumable")
	void Use(AActor* Target);
	virtual void Use_Implementation(AActor* Target);
	
	// 사용시 효과 메시지/설명
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Consumable")
	FText EffectDesc;
};
