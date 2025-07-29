// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/TRifle.h"


ATRifle:: ATRifle()
{
	Damage = 15;
	MaxAmmo = 30;
	CurrentAmmo = 30;
	FireRate = 0.2f;
	ReloadTime = 1.5f;
	Range = 600.0f;
}



void ATRifle::Fire()
{
	WeaponType = EWeaponType::Rifle;
	ATWeaponBase::Fire();
	
}


void ATRifle::Reload()
{
	WeaponType = EWeaponType::Rifle;
	ATWeaponBase::Reload();
	
}