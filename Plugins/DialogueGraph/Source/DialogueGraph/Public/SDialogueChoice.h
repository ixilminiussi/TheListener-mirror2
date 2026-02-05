#pragma once

#include "SGraphPin.h"
#include "Widgets/SCompoundWidget.h"

class SDialogueChoice : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SDialogueChoice) {}
		SLATE_ARGUMENT(TSharedPtr<SGraphPin>,GraphPin)
		SLATE_ARGUMENT(FText,LabelText)
		SLATE_ARGUMENT(bool,Compact)
		SLATE_EVENT(FOnTextCommitted,OnTextCommited)
		SLATE_ATTRIBUTE(FText,Text)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};