#include "TCapturePoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Gimmick/TMovingWall.h"
#include "Engine/StaticMeshActor.h" 
#include "TGameMode.h"
#include "Team02.h"


ATCapturePoint::ATCapturePoint()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	CaptureAreaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CaptureAreaMesh"));
	CaptureAreaMesh -> SetupAttachment(Root);
	CaptureAreaCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("CaptureAreaCollider"));
	CaptureAreaCollider->SetupAttachment(Root);
	CaptureAreaCollider->SetCollisionResponseToChannel(ECC_ATTACK, ECR_Ignore);
	CaptureAreaCollider->OnComponentBeginOverlap.AddDynamic(this, &ATCapturePoint::OnOverlapBegin);
	CaptureAreaCollider->OnComponentEndOverlap.AddDynamic(this, &ATCapturePoint::OnOverlapEnd);

	
	CapturePercent = 0.0f;
	bPlayerInArea = false;
	bEnemyInArea = false;

}

void ATCapturePoint::BeginPlay()
{
	Super::BeginPlay();
	GM = GetWorld()->GetAuthGameMode<ATGameMode>();
	
}

void ATCapturePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsCaptured && bPlayerInArea && !bEnemyInArea && CapturePercent < 100.f)
	{
		CapturePercent += DeltaTime * CaptureSpeed;
		CapturePercent = FMath::Clamp(CapturePercent, 0.f, 100.f);

		

		if (CapturePercent >= 100.f)
		{
			CompleteCapture(); // << 여기서만 한 번 호출!
		}
	}

	
}

void ATCapturePoint::CompleteCapture()
{
	if (bIsCaptured) return; // 이미 완료된 경우 중복 방지

	bIsCaptured = true;
	CapturePercent = 100.f;

	if (GM)
	{

		GM->OnCapturePointCompleted();
		GM->LastCapturedPoint = this;
	}

	
	DestroyAssignedMeshesIfNeeded();

	NotifyWall(); // 벽 열기 등 연계 행동
}

void ATCapturePoint::NotifyWall()
{
	for (ATMovingWall* Wall : LinkedWalls)
	{
		if (Wall)    // nullptr 안전체크
		{
			Wall->RotateStep(); // 벽 열기
		}
	}
}
void ATCapturePoint::DestroyAssignedMeshesIfNeeded()
{
	// 1거점에서만 실행
	if (ZoneIndex != 0) return;

	for (AStaticMeshActor* MeshActor : MeshesToDestroyOnComplete)
	{
		if (IsValid(MeshActor))
		{
			MeshActor->Destroy();
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
		if (GM && !GM->bIsWaveActive)   // 이미 웨이브 중이면 패스
		{
			GM->OnZoneOverlap(ZoneIndex);  
		}
	}
	else if (OtherActor->ActorHasTag(TEXT("Enemy")))
	{
		bEnemyInArea = true; // << 적이 들어오면 true
	}
}

void ATCapturePoint::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		bPlayerInArea = false;
	}
	else if (OtherActor->ActorHasTag(TEXT("Enemy")))
	{
		bEnemyInArea = false; // << 적이 나가면 false
	}
}