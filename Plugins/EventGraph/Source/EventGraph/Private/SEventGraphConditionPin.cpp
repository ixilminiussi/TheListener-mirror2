#include "SEventGraphConditionPin.h"
#include "EventGraphDefines.h"
#include "Styling/SlateStyleRegistry.h"

const FSlateBrush* SEventGraphConditionPin::GetPinIcon() const
{
	const ISlateStyle* StyleSet = FSlateStyleRegistry::FindSlateStyle(EventGraphEditorStyle);
	return StyleSet->GetBrush("EventGraphAsset.NodePin.Background");;
}
