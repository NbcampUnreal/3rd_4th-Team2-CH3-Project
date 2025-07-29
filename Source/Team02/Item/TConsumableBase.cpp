// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TConsumableBase.h"

// Sets default values
ATConsumableBase::ATConsumableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	ConsumableType = EConsumableType::Potion;
	Amount = 50.0f;
	EffectDesc = FText::FromString(TEXT("체력을 회복합니다."));
}
void ATConsumableBase::Use_Implementation(AActor* Target)
{
	if (!Target) return;

	// 1. 소모품 타입에 따라 효과 적용 (실제 효과는 파생에서 구현, 여기서는 예시 메시지)
	FString ItemTypeString;
	switch (ConsumableType)
	{
	case EConsumableType::Potion: ItemTypeString = TEXT("포션"); break;
	//case EConsumableType::Ammo:   ItemTypeString = TEXT("탄약"); break;
	//case EConsumableType::Buff:   ItemTypeString = TEXT("버프"); break;
	default:                      ItemTypeString = TEXT("소모품"); break;
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 2.f, FColor::Yellow,
			FString::Printf(TEXT("[%s] %s를 사용했습니다! (효과: %s)"),
				*Target->GetName(),
				*ItemTypeString,
				*EffectDesc.ToString()
			)
		);
	}
}


