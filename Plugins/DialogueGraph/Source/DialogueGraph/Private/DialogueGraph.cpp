// Copyright Epic Games, Inc. All Rights Reserved.

#include "DialogueGraph.h"
#include "DialogueGraphAction.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Styling//SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "DialogueDefines.h"
#include "DialogueGraphNodeFactory.h"
#include "EdGraphUtilities.h"
#include "KismetPins/SGraphPinColor.h"
#include "EdGraph/EdGraph.h"
#include "DialogueGraphPinFactory.h"

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
	StyleSet->Set(TEXT("DialogueGraphAsset.NodePin"),  new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("NodePin"),TEXT(".png")),FVector2D(8.0,8.0)));
	StyleSet->Set(TEXT("DialogueGraphAsset.NodeBox.Background"),  new FSlateImageBrush( StyleSet->RootToContentDir(TEXT("NodeBackgroundS"),TEXT(".png")),FVector2D(256.0,256.0)));
	StyleSet->Set(TEXT("DialogueGraphAsset.Add"),  new FSlateImageBrush( StyleSet->RootToContentDir(TEXT("Add"),TEXT(".png")),FVector2D(256.0,256.0)));
	StyleSet->Set(TEXT("DialogueGraphAsset.Remove"),  new FSlateImageBrush( StyleSet->RootToContentDir(TEXT("Remove"),TEXT(".png")),FVector2D(256.0,256.0)));
	StyleSet->Set(TEXT("DialogueGraphAsset.Start"),  new FSlateImageBrush( StyleSet->RootToContentDir(TEXT("Start"),TEXT(".png")),FVector2D(256.0,256.0)));
	StyleSet->Set(TEXT("DialogueGraphAsset.Background"),  new FSlateImageBrush( StyleSet->RootToContentDir(TEXT("Background"),TEXT(".png")),FVector2D(1920.0,1080.0)));
	StyleSet->Set(TEXT("DialogueGraphAsset.Leave"),  new FSlateImageBrush( StyleSet->RootToContentDir(TEXT("Leave"),TEXT(".png")),FVector2D(128.0,128.0)));
	
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);

	PinFactory = MakeShareable(new FDialoguePinFactory);
	FEdGraphUtilities::RegisterVisualPinFactory(PinFactory);

	NodeFactory = MakeShareable(new FDialogueNodeFactory);
	FEdGraphUtilities::RegisterVisualNodeFactory(NodeFactory);
}

void FDialogueGraphModule::ShutdownModule()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);
	FEdGraphUtilities::UnregisterVisualNodeFactory(NodeFactory);
	FEdGraphUtilities::UnregisterVisualPinFactory(PinFactory);
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDialogueGraphModule, DialogueGraph)