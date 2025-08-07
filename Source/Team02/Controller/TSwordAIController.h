#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TSwordAIController.generated.h"

class UBlackboardData;
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class TEAM02_API ATSwordAIController : public AAIController
{
	GENERATED_BODY()

	friend class ATNonPlayerCharacterSword;

public:
	ATSwordAIController();

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void BeginAI(APawn* InPawn);

	void EndAI();

public:
	//경계 반원 크기
	float SwordNPCPatrolRadius;

	//AI디버깅 용
	static int32 ShowSwordAIDebug;

	//경계 시작 지점
	const FName SwordNPCStartPatrolLocationKey = TEXT("SwordNPCStartPatrolLocation");
	//경계 종료 지점
	const FName SwordNPCEndPatrolLocationKey = TEXT("SwordNPCEndPatrolLocation");



private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UBlackboardData> SwordNPCBlackboardDataAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UBehaviorTree> SwordNPCBehaviorTree;
};
