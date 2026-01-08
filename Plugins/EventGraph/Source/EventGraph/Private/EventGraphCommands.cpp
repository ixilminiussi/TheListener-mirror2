#include "EventGraphCommands.h"


#define LOCTEXT_NAMESPACE "EventGraphCommands"

void FEventGraphCommands::RegisterCommands()
{
	UI_COMMAND(ToggleNodeAction, "Toggle Action","Toggle Selected Nodes Action",EUserInterfaceActionType::Button, FInputChord(EKeys::T, false, true, false, false));
}

#undef LOCTEXT_NAMESPACE
