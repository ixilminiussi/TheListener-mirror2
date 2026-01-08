#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"
#include "InputCoreTypes.h"

class FEventGraphCommands : public TCommands<FEventGraphCommands>
{
	public:
	FEventGraphCommands() :
		TCommands<FEventGraphCommands>(
			TEXT("EventGraphEditor"),
			NSLOCTEXT("Contexts","Event Graph", "Event Graph"),
			NAME_None,
			FEditorStyle::GetStyleSetName())
	{}

	TSharedPtr<FUICommandInfo>	ToggleNodeAction;

	virtual void RegisterCommands() override;
	
};

