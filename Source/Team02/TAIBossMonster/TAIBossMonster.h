#pragma once
#include "Character/TCharacterBase.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TAIBossMonster.generated.h"

UCLASS()
class TEAM02_API ATAIBossMonster : public ATCharacterBase
{
	GENERATED_BODY()

public:
	ATAIBossMonster();

	
	
protected:
	virtual void BeginPlay() override;
	
};
