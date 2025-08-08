// Fill out your copyright notice in the Description page of Project Settings.


#include "TBossSpawner.h"

#include "Team02/Spawner/TBossSpawner.h"
#include "TAIBossMonster/TAIBossMonster.h"
#include "Components/BoxComponent.h"

// Sets default values
ATBossSpawner::ATBossSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	
}

void ATBossSpawner::SpawnBoss()
{
	if (!BossClass) return;

	FVector SpawnLoc = GetActorLocation();
	FRotator SpawnRot = GetActorRotation();
	FActorSpawnParameters Params;

	GetWorld()->SpawnActor<ATAIBossMonster>(BossClass, SpawnLoc, SpawnRot, Params);
}
