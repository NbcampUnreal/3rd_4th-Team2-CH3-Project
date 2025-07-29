#include "Animation/TAnimInstance.h"
#include "Character/TNonPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UTAnimInstance::NativeInitializeAnimation()
{
	//이거 초기화 캐릭터 쪽에서 하면 이거 제거 오너는 npc가 아니기 때문.
	APawn* NPCPawn = TryGetPawnOwner();
	if (IsValid(NPCPawn) == true)
	{
		NPCCharacter = Cast<ATNonPlayerCharacter>(NPCPawn);
		NPCMovement = NPCCharacter->GetCharacterMovement();
	}
}

void UTAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	//오너 캐릭터가 있다면, NPC를 전부 오너로 바꾸어야함.
	if (IsValid(NPCCharacter) == true && IsValid(NPCMovement) == true)
	{
		//속도
		Velocity = NPCMovement->Velocity;
		//기본 지형에서 스피드
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		
		float GroundAcceleration = UKismetMathLibrary::VSizeXY(NPCMovement->GetCurrentAcceleration());
		bool bIsAcceleration = FMath::IsNearlyZero(GroundAcceleration) == false;
		bShouldMove = (KINDA_SMALL_NUMBER< GroundSpeed) && (bIsAcceleration == true);
		//공중에 있다면 공중에 있다고 설정
		bIsFalling = NPCMovement ->IsFalling();

		if (ATNonPlayerCharacter* OwnerNPC = Cast<ATNonPlayerCharacter>(NPCCharacter))
		{
			bShouldMove= KINDA_SMALL_NUMBER < GroundSpeed;
		}
	}
}

