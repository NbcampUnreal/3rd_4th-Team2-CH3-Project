#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TCapturePoint.generated.h"
class UBoxComponent;
class UStaticMeshComponent;
class ATMovingWall;
UCLASS()
class TEAM02_API ATCapturePoint : public AActor
{
	GENERATED_BODY()
    
public:
	ATCapturePoint();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	void NotifyWall();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;
	
	// 캡처 영역을 표시할 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* CaptureAreaMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* CaptureAreaCollider;

	// 캡처 진행도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CapturePercent;

	// MovingWall을 연결하는 CapturePoint 예시
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ATMovingWall*> LinkedWalls;
	
	// 플레이어가 영역 안에 있는지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bPlayerInArea;

	// 오버랩 이벤트
	UFUNCTION()
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
};