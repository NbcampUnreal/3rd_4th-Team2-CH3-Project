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

	// 스폰 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner")
	TSubclassOf<ATNonPlayerCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner")
	float SpawnInterval = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawner")
	int32 MaxSpawnCount = 10;

	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UBoxComponent* SpawnArea;

	UFUNCTION(BlueprintCallable, Category="Spawner")
	void ActivateSpawner();

	UFUNCTION(BlueprintCallable, Category="Spawner")
	void DeactivateSpawner();

	// (선택) 현재 소환된 적 개수 조회
	UFUNCTION(BlueprintCallable, Category="Spawner")
	int32 GetCurrentSpawned() const { return CurrentSpawned; }

	UFUNCTION(BlueprintCallable, Category="Spawner")
	void SetSpawnerActive(bool bEnable);

	

protected:
	virtual void BeginPlay() override;
	void SpawnEnemy();
	FVector GetRandomPointInBox() const;

	bool bIsActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawner")
	int32 CurrentSpawned = 0;
	FTimerHandle SpawnTimerHandle;
};
