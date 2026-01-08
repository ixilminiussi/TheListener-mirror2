#include "UI/Menus/MainMenuGameModeBase.h"
#include "UI/Menus/MainMenuHUD.h"

AMainMenuGameModeBase::AMainMenuGameModeBase()
{
	HUDClass = AMainMenuHUD::StaticClass();
}