#include "TMonsterHealthBarComponent.h"
#include "TMonsterHealthBarWidget.h"
#include "Character/TCharacterBase.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"


UTMonsterHealthBarComponent::UTMonsterHealthBarComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	HealthBarWidgetComponent=CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	
	LastKnownCurrentHP=-1.0f;
	LastKnownMaxHP=-1.0f;
	
}

void UTMonsterHealthBarComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ATCharacterBase>(GetOwner());
    
	// 핵심: WidgetComponent를 캐릭터의 Mesh에 직접 어태치
	if (OwnerCharacter && HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->AttachToComponent(
			OwnerCharacter->GetMesh(),  // RootComponent 대신 Mesh 사용
			FAttachmentTransformRules::KeepRelativeTransform
		);
	}
    
	InitializeHealthBar();

	//위젯 생성 이후 몬스터 이름 초기화
	SetupMonsterNameAfterWidgetCreation();
	
}

void UTMonsterHealthBarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//체력변화 감지 및 업데이트
	if (OwnerCharacter)
	{
		float CurrentHP=OwnerCharacter->GetCurrentHP();
		float MaxHP=OwnerCharacter->GetMaxHP();

		// 체력 변할떄만 업데이트
		if (CurrentHP != LastKnownCurrentHP || MaxHP != LastKnownMaxHP)
		{
			if (UTMonsterHealthBarWidget* HealthWidget=Cast<UTMonsterHealthBarWidget>(HealthBarWidgetComponent->GetUserWidgetObject()))
			{
				HealthWidget->UpdateHealthBar(CurrentHP,MaxHP);
			}
			LastKnownCurrentHP=CurrentHP;
			LastKnownMaxHP=MaxHP;
				
		}
	}
	// 체력바 뒤집히는거 방지 + y축만 회전
	if (HealthBarWidgetComponent)
	{
		APlayerController* PC=GetWorld()->GetFirstPlayerController();
		if (PC && PC->PlayerCameraManager)
		{
			FVector CameraLocation=PC->PlayerCameraManager->GetCameraLocation();
			FVector WidgetLocation= HealthBarWidgetComponent->GetComponentLocation();
			
			FVector Direction=CameraLocation-WidgetLocation;
			Direction.Z=0.0f;

			FRotator LookRotation=Direction.Rotation();
			HealthBarWidgetComponent->SetWorldRotation(LookRotation);
		}
	}
	
}

void UTMonsterHealthBarComponent::InitializeHealthBar()
{
	if (HealthBarWidgetComponent && HealthBarWidgetClass)
	{
		HealthBarWidgetComponent->SetWidgetClass(HealthBarWidgetClass);
		HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
		HealthBarWidgetComponent->SetDrawSize(FVector2D(500.0f, 200.0f));  
		HealthBarWidgetComponent->SetWorldScale3D(FVector(0.2f, 0.2f, 0.2f)); 
		HealthBarWidgetComponent->SetDrawAtDesiredSize(false);
		HealthBarWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 195.0f)); 
		
		// 항상 카메라를 바라보도록
		HealthBarWidgetComponent->SetTwoSided(true);
		
	}
}

void UTMonsterHealthBarComponent::InitializeMonsterName()
{
	if (HealthBarWidgetComponent)
	{
		if (UTMonsterHealthBarWidget* MonsterWidget = Cast<UTMonsterHealthBarWidget>(HealthBarWidgetComponent->GetUserWidgetObject()))
		{
			MonsterWidget->SetMonsterNameByType(GetOwner());
		}
		
	}
}

void UTMonsterHealthBarComponent::SetupMonsterNameAfterWidgetCreation()
{
	if (!HealthBarWidgetComponent->GetUserWidgetObject())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			SetupMonsterNameAfterWidgetCreation();
		});
		return;
	}
	InitializeMonsterName();
}
