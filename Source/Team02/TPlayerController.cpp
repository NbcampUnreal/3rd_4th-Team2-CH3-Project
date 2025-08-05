#include "TPlayerController.h"
#include "GameFramework/Pawn.h"
#include "TGameState.h"
#include "Character/TCharacterBase.h"
#include "UI/InGame/TUIManager.h"


ATPlayerController::ATPlayerController()
{
}

void ATPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//점검용 로그
	UE_LOG(LogTemp, Warning, TEXT("PlayerController BeginPlay called"));

	//UI 매니저 가쟈오기
	UIManager=GetGameInstance()->GetSubsystem<UTUIManager>();
	if (UIManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("UIManager found - creating UI"));
		UIManager->CreatePlayerUI();

		if (ATCharacterBase* PlayerChar=Cast<ATCharacterBase>(GetPawn()))
		{
			UIManager->SetPlayerCharacter(PlayerChar);
			UE_LOG(LogTemp, Warning, TEXT("Player character set"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UIManager NOT found!"));
	}
}

void ATPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}
