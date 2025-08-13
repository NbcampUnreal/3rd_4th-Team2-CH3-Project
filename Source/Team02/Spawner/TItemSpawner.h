// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TItemSpawner.generated.h"

class ATItemBase;
class UBoxComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;

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


	// 에디터에서 NS_RingCooldown 지정
	UPROPERTY(EditDefaultsOnly, Category="FX|Ring")
	UNiagaraSystem* RingCooldownSystem = nullptr;

	// 런타임에 붙일 컴포넌트
	UPROPERTY(VisibleAnywhere, Category="FX|Ring")
	UNiagaraComponent* RingFX = nullptr;

	// (선택) 반지름/두께를 User 파라미터로 넘기고 싶다면 이름 고정
	UPROPERTY(EditAnywhere, Category="FX|Ring")
	FName ParamCooldown = TEXT("User.Cooldown");   // Lifetime
	
	// === 제어 함수 ===
	void StartCooldownFX(float InSeconds);
	void StopCooldownFX();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	

	int32 CurrentCount = 0;
	FTimerHandle SpawnTimerHandle;

	void TrySpawnItem();
	
	UFUNCTION()
	void OnItemDestroyed(AActor* DestroyedActor);
};
