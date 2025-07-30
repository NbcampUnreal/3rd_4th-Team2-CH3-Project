#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TInputConfig.generated.h"

class UInputAction;

UCLASS()
class TEAM02_API UTInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputAction> Move;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputAction> Look;
};
