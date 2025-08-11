#include "Team02/Animation/AN_CheckSwordHit.h"
#include "Character/TNonPlayerCharacterSword.h"

//일반 공격하는 노티파이 함수

void UAN_CheckSwordHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp) == true)
	{
		ATNonPlayerCharacterSword* AttackingCharacter = Cast<ATNonPlayerCharacterSword>(MeshComp->GetOwner());
		if (IsValid(AttackingCharacter) ==true)
		{
			AttackingCharacter->HandleOnCheckSwordHit();
		}
	}
}

