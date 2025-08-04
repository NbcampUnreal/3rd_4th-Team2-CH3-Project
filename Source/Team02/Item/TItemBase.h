// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TItemBase.generated.h"

class USphereComponent;
class UBoxComponent;
UCLASS()
class TEAM02_API ATItemBase : public AActor
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	ATItemBase();

protected:
	

public:	
	
	// ------ 기본 아이템 속성 ------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxStackCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Root")
	USceneComponent* SceneRoot;
	// ★★ 총 본체(3D 모델) ★★
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* StaticMesh;

	// ★★ 충돌 박스(혹은 캡슐) ★★
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	UBoxComponent* Collider; // 박스 형태, 필요에 따라 UCapsuleComponent, USphereComponent 등도 가능

	UPROPERTY(VisibleAnywhere, Category="Collision")
	USphereComponent* PickupCollider;
	
	UFUNCTION()
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
