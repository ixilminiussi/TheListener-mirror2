#include "DialogueGraphAction.h"

#include "DialogueEdGraph.h"
#include "DialogueGraphAsset.h"
#include "DialogueGraphEditorApp.h"

DialogueGraphAction::DialogueGraphAction(EAssetTypeCategories::Type Category)
{
	AssetCategory = Category;
}

FText DialogueGraphAction::GetName() const
{
	return FText::FromString("Dialogue Graph");
}

FColor DialogueGraphAction::GetTypeColor() const
{
	return FColor::Green;
}

UClass* DialogueGraphAction::GetSupportedClass() const
{
	return UDialogueGraphAsset::StaticClass();
}

void DialogueGraphAction::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (UObject* Object : InObjects)
	{
		if (Object == nullptr)
		{
			continue;
		}
		UDialogueGraphAsset* Asset = Cast<UDialogueGraphAsset>(Object);
		if (Asset == nullptr) {continue;}
		UPackage* AssetPackage = Asset->GetPackage();
		if (!AssetPackage->IsFullyLoaded())
		{
			UE_LOG(LogTemp, Warning, TEXT("Dialogue Asset wasnt fully loaded !!!"));
			Asset->GetPackage()->FullyLoad();
		}
		
		if (UDialogueGraphAsset* DialogueAsset = Cast<UDialogueGraphAsset>(Object))
		{
			TSharedRef<DialogueGraphEditorApp> Editor (new DialogueGraphEditorApp());
			Editor->Init(mode, EditWithinLevelEditor, DialogueAsset);
		}
	}
}

uint32 DialogueGraphAction::GetCategories()
{
	return AssetCategory;
}


