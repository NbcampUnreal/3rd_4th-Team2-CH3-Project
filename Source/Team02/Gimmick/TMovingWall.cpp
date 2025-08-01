#include "TMovingWall.h"
#include "Components/StaticMeshComponent.h"

ATMovingWall::ATMovingWall()
{
	PrimaryActorTick.bCanEverTick = true;

	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	RootComponent = WallMesh;
}

void ATMovingWall::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();
	TargetLocation = StartLocation + Offset;
}

void ATMovingWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsOpening)
	{
		CurrentMoveTime += DeltaTime;
		float Alpha = FMath::Clamp(CurrentMoveTime / MoveTime, 0.f, 1.f);
		FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
		SetActorLocation(NewLocation);

		if (Alpha >= 1.f)
		{
			bIsOpening = false; // 이동 완료 시 중지
		}
	}
}

void ATMovingWall::OpenWall()
{
	if (!bIsOpening)
	{
		bIsOpening = true;
		CurrentMoveTime = 0.f;
	}
}