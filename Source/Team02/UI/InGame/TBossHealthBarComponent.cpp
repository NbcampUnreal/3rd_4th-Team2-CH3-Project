#include "TBossHealthBarComponent.h"
#include "TBossHealthBarWidget.h"
#include "Character/TCharacterBase.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"

UTBossHealthBarComponent::UTBossHealthBarComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// create Widget Component
	BossHealthBarWidgetComponent=CreateDefaultSubobject<UWidgetComponent>(TEXT("BossHealthBarWidget"));

	//set initialize value
	LastKnownCurrentHP=-1.0f;
	LastKnownMaxHP=-1.0f;
	
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

		UE_LOG(LogTemp,Warning,TEXT("Boss Widget attached to Char Mesh"));
	}

	InitializeBossHealthBar();
	
}


void UTBossHealthBarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 체력 변화 감지 및 업데이트
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

		BossHealthBarWidgetComponent->SetRelativeLocation(FVector(0.0f,0.0f,225.0f));

		// 보스 체력바 월드 배치크기
		BossHealthBarWidgetComponent->SetDrawSize(FVector2D(800.0f,300.0f));
		BossHealthBarWidgetComponent->SetWorldScale3D(FVector(0.3f, 0.3f, 0.3f));
		BossHealthBarWidgetComponent->SetDrawAtDesiredSize(false);

		// 체력바가 플레이어 카메라 바라보게끔
		BossHealthBarWidgetComponent->SetTwoSided(true);

		//오류 점검용 로그
		UE_LOG(LogTemp,Warning,TEXT("Boss Widget setup!"));
		
	}
}