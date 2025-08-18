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
			CompleteCapture(); 
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

void ATCapturePoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
									AActor* OtherActor,
									UPrimitiveComponent* OtherComp,
									int32 OtherBodyIndex,
									bool bFromSweep,
									const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;

	if (OtherActor->ActorHasTag(TEXT("Enemy")))
	{
		// 동일 액터의 여러 컴포넌트가 겹쳐도 한 번만 추가됨
		if (!EnemiesInArea.Contains(OtherActor))
		{
			EnemiesInArea.Add(OtherActor);

			// 죽어서 사라질 때 EndOverlap이 안 올 수 있으니 안전장치
			OtherActor->OnDestroyed.AddDynamic(this, &ATCapturePoint::OnOverlappingEnemyDestroyed);
		}

		bEnemyInArea = EnemiesInArea.Num() > 0;
		return;
	}

	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		bPlayerInArea = true;
		// 웨이브 시작 (최초 진입 때만, 혹은 조건에 따라 한 번만)
		if (GM && !GM->bIsWaveActive)   // 이미 웨이브 중이면 패스
		{
			GM->OnZoneOverlap(ZoneIndex);  
		}
		return;
	}
}

void ATCapturePoint::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
								  AActor* OtherActor,
								  UPrimitiveComponent* OtherComp,
								  int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this) return;

	if (OtherActor->ActorHasTag(TEXT("Enemy")))
	{
		// 깔끔하게 파괴 델리게이트도 해제
		OtherActor->OnDestroyed.RemoveDynamic(this, &ATCapturePoint::OnOverlappingEnemyDestroyed);

		EnemiesInArea.Remove(OtherActor);
		bEnemyInArea = EnemiesInArea.Num() > 0;
		return;
	}

	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		bPlayerInArea = false;
		return;
	}
}

void ATCapturePoint::OnOverlappingEnemyDestroyed(AActor* DestroyedActor)
{
	EnemiesInArea.Remove(DestroyedActor);
	bEnemyInArea = EnemiesInArea.Num() > 0;
}