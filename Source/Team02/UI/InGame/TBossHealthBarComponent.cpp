#include "TBossHealthBarComponent.h"
#include "TBossHealthBarWidget.h"
#include "Character/TCharacterBase.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"

UTBossHealthBarComponent::UTBossHealthBarComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// create Widget Component
	BossHealthBarWidgetComponent=CreateDefaultSubobject<UWidgetComponent>(TEXT("BossHealthBarWidget"));

	//set initialize value
	LastKnownCurrentHP=-1.0f;
	LastKnownMaxHP=-1.0f;

	// Widget 클래스 자동 로드
	static ConstructorHelpers::FClassFinder<UTBossHealthBarWidget> WidgetClassFinder(
		TEXT("/Game/UI/InGame/WBP_BossHealthBar"));
	if (WidgetClassFinder.Succeeded())
	{
		BossHealthBarWidgetClass = WidgetClassFinder.Class;
	}
}

void UTBossHealthBarComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter=Cast<ATCharacterBase>(GetOwner());

	// attach widgetcomponent for character mesh
	if (OwnerCharacter && BossHealthBarWidgetComponent)
	{
		BossHealthBarWidgetComponent->AttachToComponent(
			OwnerCharacter->GetMesh(),
			FAttachmentTransformRules::KeepRelativeTransform);
	}
	InitializeBossHealthBar();
	
}


void UTBossHealthBarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (OwnerCharacter)
	{
		float CurrentHP=OwnerCharacter->GetCurrentHP();
		float MaxHP=OwnerCharacter->GetMaxHP();

		// update when boss health is change
		if (CurrentHP != LastKnownCurrentHP || MaxHP != LastKnownMaxHP)
		{
			if (UTBossHealthBarWidget* BossWidget =Cast<UTBossHealthBarWidget>(BossHealthBarWidgetComponent->GetUserWidgetObject()))
			{
				BossWidget->UpdateHealthBar(CurrentHP,MaxHP);
			}
			LastKnownCurrentHP=CurrentHP;
			LastKnownMaxHP=MaxHP;
		}
	}
	// 보스 체력바 측면 이상하게보이는거랑 글자뒤집히는거 방지
	if (BossHealthBarWidgetComponent)
	{
		APlayerController* PC=GetWorld()->GetFirstPlayerController();
		if (PC && PC->PlayerCameraManager)
		{
			FVector CameraLocation=PC->PlayerCameraManager->GetCameraLocation();
			FVector WidgetLocation=BossHealthBarWidgetComponent->GetComponentLocation();

			//Y축만 회전
			FVector Direction=CameraLocation-WidgetLocation;
			Direction.Z=0.0f;

			FRotator LookRotation=Direction.Rotation();
			BossHealthBarWidgetComponent->SetWorldRotation(LookRotation);
		}
	}
}

void UTBossHealthBarComponent::InitializeBossHealthBar()
{
	if (BossHealthBarWidgetComponent && BossHealthBarWidgetClass)
	{
		BossHealthBarWidgetComponent->SetWidgetClass(BossHealthBarWidgetClass);
		BossHealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

		BossHealthBarWidgetComponent->SetRelativeLocation(FVector(0.0f,0.0f,190.0f));

		// 보스 체력바 월드 배치크기
		BossHealthBarWidgetComponent->SetDrawSize(FVector2D(800.0f,300.0f));
		BossHealthBarWidgetComponent->SetWorldScale3D(FVector(0.3f, 0.3f, 0.3f));
		BossHealthBarWidgetComponent->SetDrawAtDesiredSize(false);

		// 체력바가 플레이어 카메라 바라보게끔
		BossHealthBarWidgetComponent->SetTwoSided(true);
		
	}
}