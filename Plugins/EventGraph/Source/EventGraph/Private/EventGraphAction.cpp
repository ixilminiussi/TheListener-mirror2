#include "EventGraphAction.h"
#include "EventGraphAsset.h"
#include "EventGraphEditorApp.h"

EventGraphAction::EventGraphAction(EAssetTypeCategories::Type Category){
	AssetCategory = Category;
}
FText EventGraphAction::GetName() const
{
	return  FText::FromString("Event Graph");
}
FColor EventGraphAction::GetTypeColor() const {
	return FColor::Purple;
}
UClass* EventGraphAction::GetSupportedClass() const { 
	return UEventGraphAsset::StaticClass();
}
void EventGraphAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor){
	EToolkitMode::Type mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (UObject* object : InObjects)
	{
		UEventGraphAsset* Event = Cast<UEventGraphAsset>(object);
		if (Event != nullptr)
		{
			TSharedRef<EventGraphEditorApp> editor (new EventGraphEditorApp());
			editor->InitEditor(mode,EditWithinLevelEditor,Event);
		}
	}
}
uint32 EventGraphAction::GetCategories(){ 
	return AssetCategory;
}