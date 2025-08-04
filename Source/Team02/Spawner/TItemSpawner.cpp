// Fill out your copyright notice in the Description page of Project Settings.


#include "Team02/Spawner/TItemSpawner.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Item/TItemBase.h"


// Sets default values
ATItemSpawner::ATItemSpawner()
{
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = SceneRootComponent;
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMeshComp->SetupAttachment(RootComponent);
	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SpawnArea->SetupAttachment(StaticMeshComp);

	SpawnedItem = nullptr;
}

// Called when the game starts or when spawned
void ATItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	// 최초 15초 후 1회만 스폰
	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle, this,
		&ATItemSpawner::TrySpawnItem,
		SpawnInterval, false // false = 1회만!
	);
}

void ATItemSpawner::TrySpawnItem()
{
	if (IsValid(SpawnedItem)) return;
	SpawnDebugCount++;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow,
		FString::Printf(TEXT("TrySpawnItem 호출 횟수: %d"), SpawnDebugCount));

	// 2. 스폰 위치/회전 (예시: BoxComponent 중심)
	FVector Location = SpawnArea->GetComponentLocation();
	FRotator Rotation = FRotator::ZeroRotator;

	// 3. 스폰 파라미터
	FActorSpawnParameters Params;
	Params.Owner = this;

	// 4. 아이템 생성
	SpawnedItem = GetWorld()->SpawnActor<ATItemBase>(ItemClass, Location, Rotation, Params);
	if (SpawnedItem)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("OnDestroyed 바인딩!"));
		SpawnedItem->OnDestroyed.AddDynamic(this, &ATItemSpawner::OnItemDestroyed);
	}
}	

void ATItemSpawner::OnItemDestroyed(AActor* DestroyedActor)
{
	SpawnedItem = nullptr;

	// 다시 15초 후 스폰 예약 (1회만)
	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle, this,
		&ATItemSpawner::TrySpawnItem,
		SpawnInterval, false
	);
	// 디버그 메시지: 소멸됨, 타이머 재등록!
}