// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/TWeaponBase.h"
#include "TShotgun.generated.h"

UCLASS()
class TEAM02_API ATShotgun : public ATWeaponBase
{
	GENERATED_BODY()

public:
	ATShotgun();

	
	virtual void Fire() override;

	virtual void Reload() override;

	

};
