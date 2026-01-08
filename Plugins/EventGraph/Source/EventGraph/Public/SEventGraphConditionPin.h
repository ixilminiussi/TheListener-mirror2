#pragma once

#include "SGraphPin.h"

class SEventGraphConditionPin : public SGraphPin
{
	SLATE_BEGIN_ARGS(SEventGraphConditionPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPin)
	{
		GraphPinObj = InGraphPin;
		SGraphPin::Construct(SGraphPin::FArguments(), InGraphPin);
	}

	virtual const FSlateBrush* GetPinIcon() const override;

protected:
	virtual FSlateColor GetPinColor() const override
	{
		return FSlateColor(FLinearColor::White);
	}
};