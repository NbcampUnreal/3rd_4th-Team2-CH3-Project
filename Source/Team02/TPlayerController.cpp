#include "TPlayerController.h"
#include "GameFramework/Pawn.h"
<<<<<<< Updated upstream
#include "Item/TWeaponBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
=======
>>>>>>> Stashed changes

ATPlayerController::ATPlayerController()
{
}
void ATPlayerController::BeginPlay()
{
	Super::BeginPlay();
<<<<<<< Updated upstream
	
	
	
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

		if (Subsystem && SpectatorMappingContext)
		{
			Subsystem->AddMappingContext(SpectatorMappingContext, 0);
		}
	}

	if (PlayerUIWidgetClass)
	{
		UUserWidget* PlayerUIWidget = CreateWidget<UUserWidget>(this, PlayerUIWidgetClass);
		if (PlayerUIWidget)
		{
			PlayerUIWidget->AddToViewport();
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
	
=======
>>>>>>> Stashed changes
}


void ATPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}