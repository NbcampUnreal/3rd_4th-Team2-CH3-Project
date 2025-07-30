#include "TPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Pawn.h"
#include "Item/TWeaponBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ATPlayerController::ATPlayerController()
{
	
}
void ATPlayerController::BeginPlay()
{
	
	Super::BeginPlay();
	
	
	
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

		if (Subsystem && SpectatorMappingContext)
		{
			Subsystem->AddMappingContext(SpectatorMappingContext, 0);
		}
	}
	
	if (!CurrentWeapon)
	{
		
		if (DefaultWeaponClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			CurrentWeapon = GetWorld()->SpawnActor<ATWeaponBase>(DefaultWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		}
		else
		{
			UKismetSystemLibrary::PrintString(this, TEXT("ShotgunClass not set!"));
		}
	}
	
}


void ATPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(FireAction, ETriggerEvent::Triggered, this, &ATPlayerController::OnFire);
		EnhancedInput->BindAction(ReloadAction, ETriggerEvent::Started, this, &ATPlayerController::OnReload);
	}
}

void ATPlayerController::OnFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire();

		// 무기 타입 문자열 출력
		FString Msg = FString::Printf(TEXT("Current Weapon: %s"), *CurrentWeapon->GetWeaponTypeString());
		UKismetSystemLibrary::PrintString(this, Msg);
	}
	else
	{
		//UKismetSystemLibrary::PrintString(this, TEXT("ShotgunClass not set!"));
	}
}
void ATPlayerController::OnReload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();

		// 무기 타입 문자열 출력
		FString Msg = FString::Printf(TEXT("Current Weapon: %s"), *CurrentWeapon->GetWeaponTypeString());
		UKismetSystemLibrary::PrintString(this, Msg);
	}
	else
	{
		//UKismetSystemLibrary::PrintString(this, TEXT("ShotgunClass not set!"));
	}
}