#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TAIController.generated.h"

class UBackboardData;
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class TEAM02_API ATAIController : public AAIController
{
	GENERATED_BODY()

	friend class ATNonPlayerCharacter;

public:
	ATAIController();

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void BeginAI(APawn* InPawn);

	void EndAI();

public:
	//경계 반원 크기
	static const float PatrolRadius;
	//AI 디버깅용
	static int32 ShowAIDebug;

	//블랙보드 키: 경계 시작 지점
	static const FName StarPatrolPositionKey;
	//경계 종료 지점
	static const FName EndPatrolPositionKey;
	//타겟 캐릭터
	static const FName TargetCharacterKey;
	//웨이브인지 아닌지 확인하는 키
	static const FName IsInWaveKey;
	//점령지 위치
	static const FName CapturePointKey;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UBlackboardData> BlackboardDataAsset;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UBehaviorTree> BehaviorTree;

	
};
