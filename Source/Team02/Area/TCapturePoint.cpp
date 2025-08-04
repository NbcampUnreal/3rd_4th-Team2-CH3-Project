#include "TCapturePoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Gimmick/TMovingWall.h"
#include  "TGameMode.h"

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

	if (bPlayerInArea && CapturePercent < 100.f)
	{
		CapturePercent += DeltaTime * 20.f; // 초당 20% 증가 (조절 가능)
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow,
		  FString::Printf(TEXT("점령률: %.1f%%"), CapturePercent));
		
		if (CapturePercent >= 100.f)
		{
			CapturePercent = 100.f;
			NotifyWall();
		}
	}
	// 필요하다면 감소 등도 구현 가능
}

void ATCapturePoint::NotifyWall()
{
	for (ATMovingWall* Wall : LinkedWalls)
	{
		if (Wall)    // nullptr 안전체크
		{
			Wall->OpenWall(); // 벽 열기
		}
	}
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

		// 웨이브 시작 (최초 진입 때만, 혹은 조건에 따라 한 번만)
		ATGameMode* GM = Cast<ATGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			// 예시: 한 번만 시작하도록 체크(중복 방지)
			if (!GM->bIsWaveActive)
			{
				GM->StartWave(WaveIndex);
			}
		}
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