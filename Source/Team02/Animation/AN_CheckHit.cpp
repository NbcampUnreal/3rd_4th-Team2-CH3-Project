#include "Animation/AN_CheckHit.h"
#include "Character/TNonPlayerCharacter.h"

//일반 공격하는 노티파이 함수

void UAN_CheckHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp) == true)
	{
		ATNonPlayerCharacter* AttackingCharacter = Cast<ATNonPlayerCharacter>(MeshComp->GetOwner());
		if (IsValid(AttackingCharacter) ==true)
		{
			AttackingCharacter->HandleOnCheckHit();
		}
	}
}

