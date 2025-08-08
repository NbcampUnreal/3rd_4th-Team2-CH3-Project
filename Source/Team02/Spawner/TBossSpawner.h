// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TBossSpawner.generated.h"

class ATAIBossMonster;
class UBoxComponent ;
UCLASS()
class TEAM02_API ATBossSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATBossSpawner();
	
	void SpawnBoss();

	UBoxComponent* BoxComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<ATAIBossMonster> BossClass;
};