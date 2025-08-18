#include "TPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Character/TCharacterBase.h"
#include "UI/InGame/TUIManager.h"


ATPlayerController::ATPlayerController()
{
}

void ATPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어 입력을 게임 전용 모드로 설정 (UI 입력 차단, 마우스 커서 숨김)
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	//UI 매니저 가쟈오기
	UIManager=GetGameInstance()->GetSubsystem<UTUIManager>();
	if (UIManager)
	{
		UIManager->CreatePlayerUI();

		if (ATCharacterBase* PlayerChar=Cast<ATCharacterBase>(GetPawn()))
		{
			UIManager->SetPlayerCharacter(PlayerChar);
		}
	}
}

void ATPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}
