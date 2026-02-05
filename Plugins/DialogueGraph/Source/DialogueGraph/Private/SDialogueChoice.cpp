#include "SDialogueChoice.h"

void SDialogueChoice::Construct(const FArguments& InArgs)
{
	
	ChildSlot
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.MinWidth(22)
			.MaxWidth(22)
			[
				SNew(STextBlock)
				.ColorAndOpacity(FSlateColor(FLinearColor::White))
				.Font(FAppStyle::GetFontStyle("BoldFont"))
				.Text(InArgs._LabelText)
			]
			+SHorizontalBox::Slot()
			.MinWidth(InArgs._Compact ? 0 : 175)
			.MaxWidth(InArgs._Compact ? 0 : 175)
			.HAlign(HAlign_Left)
			[
				SNew(SEditableTextBox)
				.Text(InArgs._Text)
				.Visibility(InArgs._Compact ? EVisibility::Collapsed : EVisibility::Visible)
				.MinDesiredWidth(175)
				.OnTextCommitted(InArgs._OnTextCommited)
			]
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			[
				InArgs._GraphPin.ToSharedRef()
			]
		];
}
