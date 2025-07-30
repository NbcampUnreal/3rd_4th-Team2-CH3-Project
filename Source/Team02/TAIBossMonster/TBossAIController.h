#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TBossAIController.generated.h"

class UBlackboardData;
class UBehaviorTree;

UCLASS()
class TEAM02_API ATBossAIController : public AAIController
{
	GENERATED_BODY()
public:
	ATBossAIController();

	static int32 ShowAIDebug;
	
	// FTimerHandle PatrolTimerHandle=FTimerHandle();
	// //몬스터 클래스에서 가상함수로 빼고 리턴값만 변경해서 오버라이드 
	//   static const float PatrolRepeatInterval;
	   static const float PatrolRadius;
	static const FName StartPatrolPositionKey;
	static const FName EndPatrolPositionKey;
	

	protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void BeginAI(APawn* InPawn);
	void EndAI();
	
	 private:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess))
	TObjectPtr<UBlackboardData> BlackboardData;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess))
	TObjectPtr<UBehaviorTree> BehaviorTree;
	// void BossPatrolTimerElapsed();
};
