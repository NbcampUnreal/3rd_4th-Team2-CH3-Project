#include "TPlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/TouchInterface.h"
#include "Character/TCharacterBase.h"
#include "UI/InGame/TUIManager.h"


ATPlayerController::ATPlayerController()
{
}

void ATPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("=== PlayerController BeginPlay ==="));

	// 입력 설정만 여기서
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	// 터치 인터페이스만 유지 (이건 안전)
	if (GetWorld() && GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		UTouchInterface* TouchInterface =
			LoadObject<UTouchInterface>(nullptr, TEXT("/Game/Input/TouchInterfaceSetup.TouchInterfaceSetup"));

		if (TouchInterface)
		{
			ActivateTouchInterface(TouchInterface);
		}
	}
}
void ATPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Warning, TEXT("=== OnPossess SAFE INIT ==="));

	if (!IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not LocalController - skip UI"));
		return;
	}

	if (!GetGameInstance())
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance NULL"));
		return;
	}

	UIManager = GetGameInstance()->GetSubsystem<UTUIManager>();
	if (!UIManager)
	{
		UE_LOG(LogTemp, Error, TEXT("UIManager NULL"));
		return;
	}

	// (중요) 중복 생성 방지: UIManager가 이미 UI를 만들었으면 CreatePlayerUI 내부에서 return 하도록 권장
	UIManager->CreatePlayerUI(this);
	UE_LOG(LogTemp, Warning, TEXT("CreatePlayerUI called"));

	if (ATCharacterBase* PlayerChar = Cast<ATCharacterBase>(InPawn))
	{
		UIManager->SetPlayerCharacter(PlayerChar);
	}
}
void ATPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}
