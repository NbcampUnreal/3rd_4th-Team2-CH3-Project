#include "TPlayerController.h"
#include "GameFramework/Pawn.h"
#include "TGameState.h"
#include "TGameMode.h"
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

// // 게임 오버 UI로부터 리스폰 요청하는 코드
void ATPlayerController::RequestRespawnFromUI()
{
	if (ATGameMode* GM = Cast<ATGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->RespawnPlayer(this);
	}
}
