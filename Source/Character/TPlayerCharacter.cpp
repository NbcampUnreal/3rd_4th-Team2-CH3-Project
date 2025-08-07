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
#include "Kismet/GameplayStatics.h"
#include "TGameMode.h"

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
    // (1) 카메라 위치/회전 얻기
    FVector CameraLoc;
    FRotator CameraRot;
    GetActorEyesViewPoint(CameraLoc, CameraRot); // 이 함수가 SpringArm, 카메라 위치 사용

    // (2) 카메라에서 크로스헤어 방향으로 트레이스 (먼 거리)
    float MaxTraceDist = 10000.f;
    FVector TraceEnd = CameraLoc + (CameraRot.Vector() * MaxTraceDist);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(CurrentWeapon);

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, CameraLoc, TraceEnd, ECC_Visibility, Params);

    FVector AimTarget = bHit ? Hit.ImpactPoint : TraceEnd;

    // (3) 총구 위치 얻기
    FVector MuzzleLoc = CurrentWeapon->MuzzlePoint->GetComponentLocation();

    // (4) 총구 → AimTarget 방향 구하기
    FVector FireDir = (AimTarget - MuzzleLoc).GetSafeNormal();

    // (5) 무기에게 발사 명령 (위치, 방향 넘기기)
    CurrentWeapon->FireFrom(MuzzleLoc, FireDir); 

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (IsValid(AnimInstance) == true)
    {
      if (AnimInstance->Montage_IsPlaying(CurrentWeapon->GetAttackMontage()) == false)
      {
        AnimInstance->Montage_Play(CurrentWeapon->GetAttackMontage());
      }
    }
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

// 게임 일시정지 함수
void ATPlayerCharacter::OnPause(const FInputActionValue& InValue)
{
  if (UGameplayStatics::IsGamePaused(GetWorld())) return;

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
      if (TSharedPtr<SWidget> FocusWidget = PauseMenuInstance->TakeWidget())
      {
        InputMode.SetWidgetToFocus(FocusWidget.ToSharedRef());
      }
      InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
      PC->SetInputMode(InputMode);
      PC->bShowMouseCursor = true;
    }
  }
}

// 게임 재개 함수
void ATPlayerCharacter::ResumeGame()
{
  if (APlayerController* PC = Cast<APlayerController>(GetController()))
  {
    if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
    {
      PauseMenuInstance->RemoveFromParent();
    }
    PauseMenuInstance = nullptr;

    UGameplayStatics::SetGamePaused(GetWorld(), false);

    FInputModeGameOnly InputMode;
    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = false;
  }
}

void ATPlayerCharacter::HandleOnPostCharacterDead()
{
  // 1. 죽은 직후, 입력/이동 등 완전히 비활성화
  DisableInput(Cast<APlayerController>(GetController()));
  GetCharacterMovement()->DisableMovement();

  // 2. 이펙트/애니메이션 등 사망 연출 (선택)
  // ex) 죽는 애니메이션, 이펙트

  // 3. 일정 시간 뒤에 Respawn 트리거 (ex: 2초 후)
  FTimerHandle RespawnTimerHandle;
  GetWorld()->GetTimerManager().SetTimer(
      RespawnTimerHandle,
      this, &ATPlayerCharacter::RequestRespawn,
      2.0f, false
  );
}


void ATPlayerCharacter::RequestRespawn()
{
  // 컨트롤러 소유자 구하기
  AController* PlayerController = GetController();

  // 1. 게임모드에 RespawnPlayer 요청 (LastCapturedPoint 위치)
  if (GetWorld())
  {
    ATGameMode* GM = Cast<ATGameMode>(GetWorld()->GetAuthGameMode());
    if (GM && PlayerController)
    {
      GM->RespawnPlayer(PlayerController);
    }
  }

  // 2. 본인은 제거
  Destroy();
}

