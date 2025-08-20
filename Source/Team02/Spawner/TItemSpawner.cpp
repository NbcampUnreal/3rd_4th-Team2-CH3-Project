// Fill out your copyright notice in the Description page of Project Settings.


#include "Team02/Spawner/TItemSpawner.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
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
	
	

	if (RingCooldownSystem && !RingFX)
	{
		// 위에서 내려다보는 링이면 -90 Pitch
		RingFX = UNiagaraFunctionLibrary::SpawnSystemAttached(
			RingCooldownSystem,
			GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			FRotator(-90.f, 0.f, 0.f),
			EAttachLocation::KeepRelativeOffset,
			false    // bAutoDestroy
		);

		if (RingFX)
		{
			RingFX->SetVisibility(false, true); // 기본 비활성 표시
		}
	}

	// 최초 15초 후 1회만 스폰
	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle, this,
		&ATItemSpawner::TrySpawnItem,
		SpawnInterval, false // false = 1회만!
	);

	// 첫 스폰까지 대기형이면 쿨다운 링부터 시작
	StartCooldownFX(SpawnInterval);
}


void ATItemSpawner::StartCooldownFX(float InSeconds)
{
	if (!RingFX) return;

	// System User.Cooldown → Emitter에서 Lifetime으로 바인딩돼 있어야 함
	RingFX->SetNiagaraVariableFloat("Progress", InSeconds);

	// 시스템을 0초부터 다시 시작
	RingFX->ReinitializeSystem();
	RingFX->SetVisibility(true, true);
}

void ATItemSpawner::StopCooldownFX()
{
	if (!RingFX) return;
	RingFX->SetVisibility(false, true);
	RingFX->Deactivate();
}

void ATItemSpawner::TrySpawnItem()
{
	if (IsValid(SpawnedItem)) return;
	

	// 2. 스폰 위치/회전 (예시: BoxComponent 중심)
	FVector Location = SpawnArea->GetComponentLocation();
	FRotator Rotation = FRotator::ZeroRotator;

	// 3. 스폰 파라미터
	FActorSpawnParameters Params;
	Params.Owner = this;
	

	StopCooldownFX();
	// 4. 아이템 생성
	SpawnedItem = GetWorld()->SpawnActor<ATItemBase>(ItemClass, Location, Rotation, Params);
	if (SpawnedItem)
	{
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

	// 다음 스폰까지 쿨다운 링 재시작
	StartCooldownFX(SpawnInterval);
}

