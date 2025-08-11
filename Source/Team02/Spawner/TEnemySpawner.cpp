// Fill out your copyright notice in the Description page of Project Settings.


// Fill out your copyright notice in the Description page of Project Settings.

#include "Team02/Spawner/TEnemySpawner.h"
#include "TimerManager.h"
#include "Character/TNonPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

// 생성자
ATEnemySpawner::ATEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = RootComp;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    MeshComp->SetupAttachment(RootComponent);

    SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
    SpawnArea->SetupAttachment(MeshComp);
}

void ATEnemySpawner::BeginPlay()
{
    Super::BeginPlay();
    // 스포너는 기본적으로 비활성 상태(원하면 여기서 Activate 가능)
}

void ATEnemySpawner::ActivateSpawner()
{
    // 이미 활성화 상태이거나, 배열이 비었으면 종료
    if (bIsActive || EnemyClasses.Num() == 0)
        return;

    // 배열 안에 하나라도 null이면 종료
    for (const auto& ClassType : EnemyClasses)
    {
        if (!ClassType) // nullptr이면
            return;
    }

    bIsActive = true;
    CurrentSpawned = 0;

    GetWorld()->GetTimerManager().SetTimer(
        SpawnTimerHandle,
        this,
        &ATEnemySpawner::SpawnEnemy,
        SpawnInterval,
        true
    );
}

void ATEnemySpawner::DeactivateSpawner()
{
    if (!bIsActive) return;

    bIsActive = false;
    GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
}

void ATEnemySpawner::SpawnEnemy()
{
    if (!bIsActive || EnemyClasses.Num() == 0) return;
    if (CurrentSpawned >= MaxSpawnCount)
    {
        DeactivateSpawner();
        return;
    }

    // 랜덤 적 클래스 선택
    int32 Index = FMath::RandRange(0, EnemyClasses.Num() - 1);
    TSubclassOf<ATCharacterBase> SelectedClass = EnemyClasses[Index];

    FVector SpawnLoc = GetRandomPointInBox();
    FRotator SpawnRot = GetActorRotation();

    ATCharacterBase* Enemy = GetWorld()->SpawnActor<ATCharacterBase>(SelectedClass, SpawnLoc, SpawnRot);

    if (Enemy)
    {
        ++CurrentSpawned;
    }
}

FVector ATEnemySpawner::GetRandomPointInBox() const
{
    FVector Origin = SpawnArea->Bounds.Origin;
    FVector Extent = SpawnArea->GetScaledBoxExtent();

    FVector RandomLocal(
        FMath::FRandRange(-Extent.X, Extent.X),
        FMath::FRandRange(-Extent.Y, Extent.Y),
        FMath::FRandRange(-Extent.Z, Extent.Z)
    );

    return Origin + RandomLocal;
}

void ATEnemySpawner::SetSpawnerActive(bool bEnable)
{
    if (bEnable)
        ActivateSpawner();
    else
        DeactivateSpawner();
}