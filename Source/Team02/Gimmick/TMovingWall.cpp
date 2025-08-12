#include "TMovingWall.h"
#include "Components/StaticMeshComponent.h"

ATMovingWall::ATMovingWall()
{
	Hinge = CreateDefaultSubobject<USceneComponent>(TEXT("Hinge"));
	RootComponent = Hinge;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(Hinge);
}


void ATMovingWall::RotateStep()
{
	const float DeltaYaw = StepDegrees * (float)Direction;
	AddActorLocalRotation(FRotator(0.f, DeltaYaw, 0.f));
}

void ATMovingWall::BeginPlay()
{
	Super::BeginPlay();
	StartYaw = GetActorRotation().Yaw;
	TargetYaw = 0.f; // 시작을 0으로 보고 누적
}

void ATMovingWall::AddStep()
{
	TargetYaw += StepDegrees * (float)Direction;
}

void ATMovingWall::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 현재 로컬 기준 Yaw로 맞춰도 되고, 월드Yaw 기준으로 간단히 처리
	FRotator Cur = GetActorRotation();
	FRotator Desired = FRotator(Cur.Pitch, StartYaw + TargetYaw, Cur.Roll);
	FRotator NewRot = FMath::RInterpTo(Cur, Desired, DeltaSeconds, InterpSpeed);
	SetActorRotation(NewRot);
}