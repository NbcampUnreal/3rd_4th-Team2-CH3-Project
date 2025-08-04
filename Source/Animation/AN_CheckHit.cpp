#include "Animation/AN_CheckHit.h"
#include "Character/TCharacterBase.h"

//일반 공격하는 노티파이 함수

void UAN_CheckHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp) == true)
	{
		ATCharacterBase* AttackingCharacter = Cast<ATCharacterBase>(MeshComp->GetOwner());
		if (IsValid(AttackingCharacter) ==true)
		{
			AttackingCharacter->HandleOnCheckHit();
		}
	}
}

