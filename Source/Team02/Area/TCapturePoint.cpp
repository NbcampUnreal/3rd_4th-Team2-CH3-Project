#include "TCapturePoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"

ATCapturePoint::ATCapturePoint()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	CaptureAreaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CaptureAreaMesh"));
	CaptureAreaMesh -> SetupAttachment(Root);
	CaptureAreaCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("CaptureAreaCollider"));
	CaptureAreaCollider->SetupAttachment(Root);

	CaptureAreaCollider->OnComponentBeginOverlap.AddDynamic(this, &ATCapturePoint::OnOverlapBegin);
	CaptureAreaCollider->OnComponentEndOverlap.AddDynamic(this, &ATCapturePoint::OnOverlapEnd);

	CapturePercent = 0.0f;
	bPlayerInArea = false;
}

void ATCapturePoint::BeginPlay()
{
	Super::BeginPlay();
}

void ATCapturePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPlayerInArea)
	{
		// 예시: 초당 10%씩 증가
		CapturePercent = FMath::Min(CapturePercent + 10.f * DeltaTime, 100.f);
	}
	// 필요하다면 감소 등도 구현 가능
}

void ATCapturePoint::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag(TEXT("Player")))
	{
		bPlayerInArea = true;
	}
}

void ATCapturePoint::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->ActorHasTag(TEXT("Player")))
	{
		bPlayerInArea = false;
	}
}