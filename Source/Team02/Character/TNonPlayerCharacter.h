#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/TCharacterBase.h"
#include "TNonPlayerCharacter.generated.h"

UCLASS()
class TEAM02_API ATNonPlayerCharacter : public ATCharacterBase
{
	GENERATED_BODY()

public:
	ATNonPlayerCharacter();

	virtual void BeginPlay() override;
	
};
