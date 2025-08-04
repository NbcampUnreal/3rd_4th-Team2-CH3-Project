#include "Character/TPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Input/TInputConfig.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/TWeaponBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ATPlayerCharacter::ATPlayerCharacter()

{
  PrimaryActorTick.bCanEverTick = true;

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
      UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

      if (Subsystem && PlayerCharacterInputMappingContext)
      {
        Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
      }
    }
  }

  if (DefaultWeaponClass)
  {
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();

    // 손 소켓에 붙이려면 Spawn 위치/회전은 대략 캐릭터 위치로
    FVector SpawnLoc = GetMesh()->GetSocketLocation(TEXT("Hand_R_Socket"));
    FRotator SpawnRot = GetMesh()->GetSocketRotation(TEXT("Hand_R_Socket"));

    // 무기 액터 스폰
    CurrentWeapon = GetWorld()->SpawnActor<ATWeaponBase>(
      DefaultWeaponClass, SpawnLoc, SpawnRot, SpawnParams);

    if (CurrentWeapon)
    {
      // 손에 Attach
      CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                                       TEXT("Hand_R_Socket"));
      CurrentWeapon->SetActorHiddenInGame(false);
      CurrentWeapon->SetActorEnableCollision(false);
    }
  }
}

void ATPlayerCharacter::Tick(float DeltaSeconds)
{
  Super::Tick(DeltaSeconds);

  CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaSeconds, 35.f);
  CameraComponent->SetFieldOfView(CurrentFOV);
}

void ATPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);

  UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
  if (IsValid(EnhancedInputComponent) == true)
  {
    EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Move, ETriggerEvent::Triggered, this,
                                       &ThisClass::InputMove);
    EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Look, ETriggerEvent::Triggered, this,
                                       &ThisClass::InputLook);
    EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Jump, ETriggerEvent::Triggered, this,
                                       &ThisClass::Jump);
    EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->FireAction, ETriggerEvent::Triggered, this,
                                       &ThisClass::OnFire);
    EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->ReloadAction, ETriggerEvent::Started, this,
                                       &ThisClass::OnReload);
    EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Zoom, ETriggerEvent::Started, this,
                                       &ThisClass::InputStartZoom);
    EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Zoom, ETriggerEvent::Completed, this,
                                       &ThisClass::InputEndZoom);
    EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Pause, ETriggerEvent::Triggered, this, &ThisClass::OnPause);
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

void ATPlayerCharacter::InputStartZoom(const FInputActionValue& InputActionValue)
{
  TargetFOV = 45.f;
}

void ATPlayerCharacter::InputEndZoom(const FInputActionValue& InputActionValue)
{
  TargetFOV = 70.f;
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
void ATPlayerCharacter::OnPause(const FInputActionValue& InValue)
{
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
                if (!PauseMenuInstance && PauseMenuClass)
                {
                        PauseMenuInstance = CreateWidget<UUserWidget>(PC, PauseMenuClass);
                }

                if (PauseMenuInstance && !PauseMenuInstance->IsInViewport())
                {
                        PauseMenuInstance->AddToViewport();

                        UGameplayStatics::SetGamePaused(GetWorld(), true);

                        FInputModeUIOnly InputMode;
                        InputMode.SetWidgetToFocus(PauseMenuInstance->TakeWidget());
                        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                        PC->SetInputMode(InputMode);
                        PC->bShowMouseCursor = true;
                }
        }
}

// 게임 재개 함수 (C++에서 호출용)
void ATPlayerCharacter::RestartGame()
{
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
                if (PauseMenuInstance)
                {
                        PauseMenuInstance->RemoveFromParent();
                        PauseMenuInstance = nullptr;
                }

                UGameplayStatics::SetGamePaused(GetWorld(), false);

                FInputModeGameOnly InputMode;
                PC->SetInputMode(InputMode);
                PC->bShowMouseCursor = false;
        }
}

