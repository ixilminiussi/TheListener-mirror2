// Copyright Epic Games, Inc. All Rights Reserved.

#include "EventGraph.h"
#include "EventGraphAction.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Styling//SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "EventGraphDefines.h"
#include "EdGraphUtilities.h"
#include "KismetPins/SGraphPinColor.h"
#include "EdGraph/EdGraph.h"
#include "EventGraphPinFactory.h"
#include "EventNodeFactory.h"

#define LOCTEXT_NAMESPACE "FEventGraphModule"

void FEventGraphModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	IAssetTools& AssetTools = IAssetTools::Get();
	EAssetTypeCategories::Type AssetType = AssetTools.RegisterAdvancedAssetCategory(EventGraphCategoryKey,FText::FromString("Event Graph"));
	TSharedPtr<EventGraphAction> EGraphAction = MakeShareable(new EventGraphAction(AssetType));
	AssetTools.RegisterAssetTypeActions(EGraphAction.ToSharedRef());

	StyleSet = MakeShareable(new FSlateStyleSet(EventGraphEditorStyle));
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin("EventGraph");
	FString ContentDir = Plugin->GetContentDir();
	StyleSet->SetContentRoot(ContentDir);
	
	StyleSet->Set(TEXT("AddPin.EventGraphAsset"),  new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("AddPin"),TEXT(".png")),FVector2D(128.0,128.0)));
	StyleSet->Set(TEXT("ToggleNode.EventGraphAsset"),  new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("ToggleNode"),TEXT(".png")),FVector2D(128.0,128.0)));
	StyleSet->Set(TEXT("RemovePin.EventGraphAsset"),  new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("RemovePin"),TEXT(".png")),FVector2D(128.0,128.0)));
	StyleSet->Set(TEXT("ClassThumbnail.EventGraphAsset"),  new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Icon"),TEXT(".png")),FVector2D(128.0,128.0)));
	StyleSet->Set(TEXT("ClassIcon.EventGraphAsset"),  new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Icon"),TEXT(".png")),FVector2D(128.0,128.0)));
	StyleSet->Set(TEXT("EventGraphAsset.NodePin.Background"),  new FSlateImageBrush( StyleSet->RootToContentDir(TEXT("NodePin"),TEXT(".png")),FVector2D(8.0,8.0)));
	StyleSet->Set(TEXT("EventGraphAsset.NodeBox.Background"),  new FSlateImageBrush( StyleSet->RootToContentDir(TEXT("NodeBackgroundS"),TEXT(".png")),FVector2D(256.0,256.0)));
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);

	PinFactory = MakeShareable(new FEventPinFactory);
	FEdGraphUtilities::RegisterVisualPinFactory(PinFactory);

	EventNodeFactory = MakeShareable(new FEventNodeFactory());
	FEdGraphUtilities::RegisterVisualNodeFactory(EventNodeFactory);
}

void FEventGraphModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);
	FEdGraphUtilities::UnregisterVisualPinFactory(PinFactory);
	PinFactory.Reset();
	FEdGraphUtilities::UnregisterVisualNodeFactory(EventNodeFactory);
	EventNodeFactory.Reset();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEventGraphModule, EventGraph)