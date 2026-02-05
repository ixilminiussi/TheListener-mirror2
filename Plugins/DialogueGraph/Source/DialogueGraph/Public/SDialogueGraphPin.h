#pragma once

#include "SGraphPin.h"

class SDialogueGraphPin : public SGraphPin
{
	SLATE_BEGIN_ARGS(SDialogueGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPin);
	
	virtual const FSlateBrush* GetPinIcon() const override;

protected:
	virtual FSlateColor GetPinColor() const override
	{
		return FSlateColor(FLinearColor::White);
	}

};