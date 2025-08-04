// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/TPistol.h"


ATPistol::ATPistol()
{
	Damage = 15;
	TotalAmmo = 7;
	MaxAmmo = 7;
	CurrentAmmo = 7;
	
	FireRate = 1.2f;
	ReloadTime = 1.5f;
	Range = 200.0f;

	
}


void ATPistol::Fire()
{
	WeaponType = EWeaponType::Pistol;
	ATWeaponBase::Fire();
	
}


void ATPistol::Reload()
{
	WeaponType = EWeaponType::Pistol;
	ATWeaponBase::Reload();
	
}