// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TEnemySpawner.generated.h"

class UBoxComponent;
class ATNonPlayerCharacter;
class UStaticMeshComponent;
UCLASS()
class TEAM02_API ATEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATEnemySpawner();

	// 몇 초 간격으로 몇 마리 소환할지 등
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner")
	TSubclassOf<ATNonPlayerCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner")
	float SpawnInterval = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner")
	int32 MaxSpawnCount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	USceneComponent* SceneRootComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* StaticMeshcomp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	UBoxComponent* SpawnArea;

	UFUNCTION(BlueprintCallable)
	void ActivateSpawner();

	UFUNCTION(BlueprintCallable)
	void DeactivateSpawner();


protected:
	virtual void BeginPlay() override;
	void SpawnEnemy();

	bool bIsActive = false;
	int32 CurrentSpawned = 0;

	FVector GetRandomPointInBox() const;
	FTimerHandle SpawnTimerHandle;
};
