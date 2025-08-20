#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_CheckSwordHit.generated.h"

/**
 * 
 */
UCLASS()
class TEAM02_API UAN_CheckSwordHit : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference) override;
};
