// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TItemSpawner.generated.h"

class ATItemBase;
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class TEAM02_API ATItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATItemSpawner();

	int32 SpawnDebugCount = 0;
	// 어떤 아이템을 스폰할지 지정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TSubclassOf<ATItemBase> ItemClass;

	// 몇 초 간격으로 스폰할지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	float SpawnInterval = 10.0f;

	// 최대 몇 개까지 동시에 스폰할지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	int32 MaxCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	USceneComponent* SceneRootComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* StaticMeshComp;
	// 스폰 지역(박스)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UBoxComponent* SpawnArea;
	ATItemBase* SpawnedItem;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

	int32 CurrentCount = 0;
	FTimerHandle SpawnTimerHandle;

	void TrySpawnItem();
	
	UFUNCTION()
	void OnItemDestroyed(AActor* DestroyedActor);
};
