#pragma once
#include "EdGraphUtilities.h"
#include "SEventGraphConditionPin.h"

struct FEventPinFactory : public FGraphPanelPinFactory
{
public:
	virtual ~FEventPinFactory() {}
	virtual TSharedPtr<SGraphPin> CreatePin(class UEdGraphPin* Pin) const override
	{
		if (FName(TEXT("ConditionPin")) == Pin->PinType.PinSubCategory)
		{
			return SNew(SEventGraphConditionPin,Pin);
		}
		return nullptr;
	}
};
