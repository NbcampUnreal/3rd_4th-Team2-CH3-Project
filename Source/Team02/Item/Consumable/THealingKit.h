// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TConsumableBase.h"
#include "THealingKit.generated.h"


UCLASS()
class TEAM02_API ATHealingKit : public ATConsumableBase
{
	GENERATED_BODY()

public:
	ATHealingKit();

	virtual void OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
) override;
	virtual void Use_Implementation(AActor* Target) override;
};

	
