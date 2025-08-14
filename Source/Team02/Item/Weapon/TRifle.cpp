// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/TRifle.h"


ATRifle:: ATRifle()
{
	Damage = 15;
	SetTotalAmmo(99999999);
	MaxAmmo = 100;
	CurrentAmmo = 100;
	FireRate = 0.2f;
	ReloadTime = 2.0f;
	Range = 200.0f;
}


