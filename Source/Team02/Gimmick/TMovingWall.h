
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

public:    
	

	UPROPERTY(VisibleAnywhere)
    USceneComponent* Hinge;          // 루트 = 힌지

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* DoorMesh;  // 문 메쉬(힌지에 붙임)

    UPROPERTY(EditAnywhere, Category="Door")
    float StepDegrees = 90.f;        // 한 번에 회전할 각도

    UPROPERTY(EditAnywhere, Category="Door")
    int32 Direction = +1;            // +1 이면 +10°, -1 이면 -10°

	UPROPERTY(EditAnywhere, Category="Door")
	float InterpSpeed = 6.f; // 부드럽게 도달 속도

	float TargetYaw = 0.f;   // 로컬 Yaw 목표
	float StartYaw = 0.f;
    UFUNCTION(BlueprintCallable, Category="Door")
    void RotateStep();


	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void AddStep(); // 목표각을 10°씩 늘림
};
