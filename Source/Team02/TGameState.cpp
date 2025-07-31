// TGameState.cpp

#include "TGameState.h"
//#include "TPlayerController.h"

ATGameState::ATGameState()
{
	
}

void ATGameState::BeginPlay()
{
	Super::BeginPlay();

	GameStart();

	// 위젯 갱신
	// GetWorldTimerManager().SetTimer().Timer(
	// 	HUDUpdateTimerHandle,
	// 	this,
	// 	&ATGameState::UpdateHUD,
	// 	0.1f,
	// 	true
	// 	);

}

void ATGameState::GameStart()
{
	// 플레이어 컨트롤러와 HUD와 연결하는 코드 필요

	// 게임 인스턴스를 가져오는 코드 필요

	// Count와 관련된 기능들을 초기화하는 코드 필요

	// 스폰 볼륨을 통해 아이템을 불러오는 코드 필요

	// 제한 시간이 있다면 이를 조정하는 코드 필요

	// 마지막으로 HUD 업데이트 코드 필요
}

void ATGameState::GameClear()
{
	// 보스를 잡았을 때 클리어
}

void ATGameState::GameOver()
{
	// 게임 인스턴스 코드 필요
}

void ATGameState::GameEnd()
{
	// 게임을 멈추는 코드 필요

	// 플레이어 컨트롤러에서 Restart와 Exit 버튼이 포함된 End 메뉴를 보여주는 코드 필요
	
}

void ATGameState::UpdateGameTime()
{
	
}

void ATGameState::UpdateHUD()
{
	// 위젯용 텍스트 코드 필요
}