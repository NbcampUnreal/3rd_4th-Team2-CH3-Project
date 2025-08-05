#include "Character/TPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/TInputConfig.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/TWeaponBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"

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
                        CurrentWeapon->AttachToComponent(
                            GetMesh(),
                            FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                            TEXT("Hand_R_Socket")
                        );
                        CurrentWeapon->SetActorHiddenInGame(false);
                        CurrentWeapon->SetActorEnableCollision(false);
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
                EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->ReloadAction, ETriggerEvent::Started, this, &ThisClass::OnReload);
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
                CurrentWeapon->FireFrom(MuzzleLoc, FireDir); // <<--- 새 함수 필요!

                // 기존 문자열 출력 등 유지
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