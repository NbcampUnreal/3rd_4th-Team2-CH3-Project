
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TMovingWall.generated.h"

UCLASS()
class TEAM02_API ATMovingWall : public AActor
{
	GENERATED_BODY()
    
public:    
	ATMovingWall();

protected:
	virtual void BeginPlay() override;

public:    
	virtual void Tick(float DeltaTime) override;

	// 벽(문) 열기 함수
	UFUNCTION()
	void OpenWall();

	// 이동 시작 위치/타겟 위치
	FVector StartLocation;
	FVector TargetLocation;

	// 이동 관련 변수
	bool bIsOpening = false;
	float MoveTime = 1.0f; // 이동에 걸릴 시간(초)
	float CurrentMoveTime = 0.f;

	// 에디터에서 벽(문) 메쉬 지정
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WallMesh;

	// (에디터에서 이동 거리 등 지정)
	UPROPERTY(EditAnywhere, Category="Wall Move")
	FVector Offset = FVector(200.f, 0.f, 0.f); // 오른쪽 200만큼 이동
};
