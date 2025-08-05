// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/TPistol.h"


ATPistol::ATPistol()
{
	Damage = 15;
	SetTotalAmmo(100);
	MaxAmmo = 7;
	CurrentAmmo = 7;
	
	FireRate = 1.2f;
	ReloadTime = 1.5f;
	Range = 200.0f;

	
}



