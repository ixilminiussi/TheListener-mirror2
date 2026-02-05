#pragma once
#include "EdGraphUtilities.h"
#include "SDialogueGraphPin.h"

struct FDialoguePinFactory : public FGraphPanelPinFactory
{
public:
	virtual ~FDialoguePinFactory() {};
	virtual TSharedPtr<SGraphPin> CreatePin(class UEdGraphPin* Pin) const override
	{
		if (FName(TEXT("DialoguePin")) == Pin->PinType.PinSubCategory)
		{
			return SNew(SDialogueGraphPin,Pin);
		}
		return nullptr;
	}
};