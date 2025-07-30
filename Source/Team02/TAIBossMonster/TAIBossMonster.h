#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TAIBossMonster.generated.h"

UCLASS()
class TEAM02_API ATAIBossMonster : public ACharacter
{
	GENERATED_BODY()

public:
	ATAIBossMonster();

	
	
protected:
	virtual void BeginPlay() override;
	
};
