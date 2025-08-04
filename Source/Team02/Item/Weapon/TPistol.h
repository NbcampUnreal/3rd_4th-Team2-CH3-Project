// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TWeaponBase.h"
#include "TPistol.generated.h"

class USphereComponent;
class UBoxComponent;

UCLASS()
class TEAM02_API ATPistol : public ATWeaponBase
{
	GENERATED_BODY()
public:
	ATPistol();

	
	
	virtual void Fire() override;

	virtual void Reload() override;

	

	
};
