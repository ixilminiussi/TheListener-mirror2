#include "SDialogueGraphPin.h"

#include "DialogueDefines.h"
#include "DialogueLineDataFuture.h"
#include "Components/SlateWrapperTypes.h"
#include "Styling/SlateStyleRegistry.h"

void SDialogueGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPin)
{
	GraphPinObj = InGraphPin;
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPin);
}

const FSlateBrush* SDialogueGraphPin::GetPinIcon() const
{
	const ISlateStyle* StyleSet = FSlateStyleRegistry::FindSlateStyle(DialogueGraphEditorStyle);
	return StyleSet->GetBrush("DialogueGraphAsset.NodePin");;
}

