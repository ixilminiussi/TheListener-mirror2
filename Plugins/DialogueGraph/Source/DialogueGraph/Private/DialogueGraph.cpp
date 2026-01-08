// Copyright Epic Games, Inc. All Rights Reserved.

#include "DialogueGraph.h"
#include "DialogueGraphAction.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Styling//SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "DialogueDefines.h"
#include "EdGraphUtilities.h"
#include "KismetPins/SGraphPinColor.h"
#include "EdGraph/EdGraph.h"

#define LOCTEXT_NAMESPACE "FDialogueGraphModule"

void FDialogueGraphModule::StartupModule()
{
	IAssetTools& AssetTools = IAssetTools::Get();
	EAssetTypeCategories::Type AssetType = AssetTools.RegisterAdvancedAssetCategory(DialogueGraphCategoryKey,FText::FromString("Dialogue Graph"));
	DGraphAction = MakeShareable(new DialogueGraphAction(AssetType));
	AssetTools.RegisterAssetTypeActions(DGraphAction.ToSharedRef());

	StyleSet = MakeShareable(new FSlateStyleSet(DialogueGraphEditorStyle));
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin("DialogueGraph");
	FString ContentDir = Plugin->GetContentDir();
	StyleSet->SetContentRoot(ContentDir);

	StyleSet->Set(TEXT("ClassThumbnail.DialogueGraphAsset"),  new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Icon"),TEXT(".png")),FVector2D(128.0,128.0)));
	StyleSet->Set(TEXT("ClassIcon.DialogueGraphAsset"),  new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Icon"),TEXT(".png")),FVector2D(128.0,128.0)));
	
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
	
}

void FDialogueGraphModule::ShutdownModule()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDialogueGraphModule, DialogueGraph)