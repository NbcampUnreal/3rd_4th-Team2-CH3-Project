// Fill out your copyright notice in the Description page of Project Settings.


#include "Team02/Spawner/TEnemySpawner.h"
#include "TimerManager.h"
#include "Character/TNonPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"

// Sets default values
ATEnemySpawner::ATEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRootComponent= CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRootComponent;
	StaticMeshcomp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshcomp -> SetupAttachment(RootComponent);
	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SpawnArea->SetupAttachment(StaticMeshcomp);
}

// Called when the game starts or when spawned
void ATEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	
}

void ATEnemySpawner::SpawnEnemy()
{
	if (!EnemyClass) return;
	
	
	if (CurrentSpawned >= MaxSpawnCount)
	{
		// 더 이상 소환하지 않음
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	FVector SpawnLoc = GetRandomPointInBox();
	FRotator SpawnRot = GetActorRotation();

	ATNonPlayerCharacter* Enemy = GetWorld()->SpawnActor<ATNonPlayerCharacter>(EnemyClass, SpawnLoc, SpawnRot);

	if (Enemy)
	{
		++CurrentSpawned;
	}
}

FVector ATEnemySpawner::GetRandomPointInBox() const
{
	FVector Origin = SpawnArea->Bounds.Origin;
	FVector Extent = SpawnArea->GetScaledBoxExtent();

	// -Extent~+Extent 범위에서 랜덤	
	FVector RandomLocal = FVector(
		FMath::FRandRange(-Extent.X, Extent.X),
		FMath::FRandRange(-Extent.Y, Extent.Y),
		FMath::FRandRange(-Extent.Z, Extent.Z)
	);

	return Origin + RandomLocal;
}

void ATEnemySpawner::ActivateSpawner()
{
	if (bIsActive) return;
	bIsActive = true;
	CurrentSpawned = 0; // 필요하면 소환수 리셋
	// 타이머 시작 (중복 방지)
	if (EnemyClass)
	{
		GetWorld()->GetTimerManager().SetTimer(
			SpawnTimerHandle, this,
			&ATEnemySpawner::SpawnEnemy,
			SpawnInterval, true
		);
	}
}

void ATEnemySpawner::DeactivateSpawner()
{
	if (!bIsActive) return;
	bIsActive = false;
	// 타이머 종료
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
}