// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Consumable/THealingKit.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Character/TPlayerCharacter.h"

ATHealingKit::ATHealingKit()
{
	ConsumableType = EConsumableType::Potion;
	Amount = 50.0f; // 회복량
	EffectDesc = FText::FromString(TEXT("체력을 회복합니다."));
}


void ATHealingKit::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// 플레이어 등 유효한 액터만 체크
	if (OtherActor && OtherActor != this &&  OtherActor->ActorHasTag(TEXT("Player")))
	{
		

		Use(OtherActor);   // 회복 적용
		Destroy();         // 아이템 파괴
	}
}




void ATHealingKit::Use_Implementation(AActor* Target) 
{
	ATPlayerCharacter* Player = Cast<ATPlayerCharacter>(Target);
	if (Player)
	{
		float BeforeHP = Player->GetCurrentHP();
		float MaxHP = Player->GetMaxHP();
		float NewHP = FMath::Clamp(BeforeHP + Amount, 0.f, MaxHP);

		Player->SetCurrentHP(NewHP);

		FString Msg = FString::Printf(TEXT("[힐킷] HP: %.0f → %.0f (MAX: %.0f)"), BeforeHP, NewHP, MaxHP);
		UKismetSystemLibrary::PrintString(this, Msg, true, true, FLinearColor::Green, 1.5f);
	}
}