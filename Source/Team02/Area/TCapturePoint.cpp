#include "TCapturePoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Gimmick/TMovingWall.h"
#include "TGameMode.h"
#include "AI/TAIController.h"
#include "Character/TNonPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
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

	if (bPlayerInArea && !bEnemyInArea && CapturePercent < 100.f)
	{
		CapturePercent += DeltaTime * CaptureSpeed; 
		CapturePercent = FMath::Clamp(CapturePercent, 0.f, 100.f);

		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow,
			FString::Printf(TEXT("점령률: %.1f%%"), CapturePercent));

		if (CapturePercent >= 100.f)
		{
			if (GM)
			{
				GM->OnCapturePointCompleted();
				GM->LastCapturedPoint = this; 
			}
			CapturePercent = 100.f;
			NotifyWall();
		}
	}
	
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
		if (GM && !GM->bIsWaveActive)   // 이미 웨이브 중이면 패스
		{
			GM->OnZoneOverlap(ZoneIndex);   // 여기서만 최초 1회!
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