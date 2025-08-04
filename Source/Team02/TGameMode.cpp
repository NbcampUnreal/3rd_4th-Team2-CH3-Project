// TGameMode.cpp

#include "TGameMode.h"
#include "TPlayerController.h"
#include "Character/TCharacterBase.h"
#include "TGameState.h"

ATGameMode::ATGameMode()
{
	PlayerControllerClass = ATPlayerController::StaticClass();
	DefaultPawnClass = ATCharacterBase::StaticClass();
}