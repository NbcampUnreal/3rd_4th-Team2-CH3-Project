#include "Character/TPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/TInputConfig.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/TWeaponBase.h"
#include "GameFramework/CharacterMovementComponent.h"

ATPlayerCharacter::ATPlayerCharacter()
{
        PrimaryActorTick.bCanEverTick = false;

        bUseControllerRotationPitch = false;
        bUseControllerRotationYaw = true;
        bUseControllerRotationRoll = false;

        GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
        GetCharacterMovement()->bOrientRotationToMovement = true;
        GetCharacterMovement()->bUseControllerDesiredRotation = false;
        GetCharacterMovement()->MaxWalkSpeed = 400.f;

        SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
        SpringArmComponent->SetupAttachment(RootComponent);
        SpringArmComponent->TargetArmLength = 190.f;
        SpringArmComponent->bUsePawnControlRotation = true;
        SpringArmComponent->bInheritPitch = true;
        SpringArmComponent->bInheritYaw = true;
        SpringArmComponent->bInheritRoll = false;
        SpringArmComponent->bDoCollisionTest = true;
        SpringArmComponent->SetRelativeLocation(FVector(0.f, 25.f, 25.f));

        CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
        CameraComponent->SetupAttachment(SpringArmComponent);
        CameraComponent->bUsePawnControlRotation = false;
}

void ATPlayerCharacter::BeginPlay()
{
        Super::BeginPlay();

        APlayerController* PlayerController = Cast<APlayerController>(GetController());
        if (IsValid(PlayerController))
        {
                if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
                {
                        UEnhancedInputLocalPlayerSubsystem* Subsystem =
                            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

                        if (Subsystem && PlayerCharacterInputMappingContext)
                        {
                                Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
                        }
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

void ATPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
        Super::SetupPlayerInputComponent(PlayerInputComponent);

        UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
        if (IsValid(EnhancedInputComponent) == true)
        {
                EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Move, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
                EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Look, ETriggerEvent::Triggered, this, &ThisClass::InputLook);
                EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Jump, ETriggerEvent::Triggered, this, &ThisClass::Jump);
                EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->FireAction, ETriggerEvent::Triggered, this, &ThisClass::OnFire);
                EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->ReloadAction, ETriggerEvent::Triggered, this, &ThisClass::OnReload);
        }
}

void ATPlayerCharacter::InputMove(const FInputActionValue& InValue)
{
        FVector2D MovementVector = InValue.Get<FVector2D>();

        const FRotator ControlRotation = GetController()->GetControlRotation();
        const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);

        const FVector ForwardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
        const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);

        AddMovementInput(ForwardVector, MovementVector.X);
        AddMovementInput(RightVector, MovementVector.Y);
}

void ATPlayerCharacter::InputLook(const FInputActionValue& InValue)
{
        FVector2D LookVector = InValue.Get<FVector2D>();

        AddControllerYawInput(LookVector.X);
        AddControllerPitchInput(LookVector.Y);
}

void ATPlayerCharacter::OnFire(const FInputActionValue& InValue)
{
        if (CurrentWeapon)
        {
                CurrentWeapon->Fire();

                // 무기 타입 문자열 출력
                FString Msg = FString::Printf(TEXT("Current Weapon: %s"), *CurrentWeapon->GetWeaponTypeString());
                UKismetSystemLibrary::PrintString(this, Msg);
        }
}

void ATPlayerCharacter::OnReload(const FInputActionValue& InValue)
{
        if (CurrentWeapon)
        {
                CurrentWeapon->Reload();

                // 무기 타입 문자열 출력
                FString Msg = FString::Printf(TEXT("Current Weapon: %s"), *CurrentWeapon->GetWeaponTypeString());
                UKismetSystemLibrary::PrintString(this, Msg);
        }
}