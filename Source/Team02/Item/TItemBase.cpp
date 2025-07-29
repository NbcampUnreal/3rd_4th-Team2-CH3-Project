// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TItemBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"


// Sets default values
ATItemBase::ATItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemName = "Default";
	ItemDescription = "아이템 설명";
	ItemID = 0;
	MaxStackCount = 1;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
	// 콜라이더 생성
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	Collider->SetupAttachment(SceneRoot);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	StaticMesh->SetupAttachment(Collider);
	
	PickupCollider = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollider"));
	PickupCollider->SetupAttachment(SceneRoot);
	PickupCollider->InitSphereRadius(50.0f); // 기본값, 아이템 크기에 맞게 조절
	PickupCollider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	PickupCollider->OnComponentBeginOverlap.AddDynamic(this, &ATItemBase::OnOverlapBegin);
}


void ATItemBase::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// 예시: 플레이어만 줍기 허용
	if (OtherActor && OtherActor != this)
	{
		// 줍기 로직 (인벤토리 추가 등)
		// ...
	}
}


