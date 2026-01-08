#include "EventGraphMainTabFactory.h"

#include "AssetViewUtils.h"
#include "EventGraphEditorApp.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"
#include "EventGraphAsset.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "EventGraphDefines.h"
#include "GraphEditor.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

EventGraphMainTabFactory::EventGraphMainTabFactory(TSharedPtr<class EventGraphEditorApp> InApp) : FWorkflowTabFactory(FName(EventGraphTab1),InApp)
{
	App = InApp;
	TabLabel = FText::FromString("Event Graph");
	ViewMenuDescription = FText::FromString("Event Graph View");
	ViewMenuTooltip = FText::FromString("Viewer for the Event Graph ");

	
}

TSharedRef<SWidget> EventGraphMainTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<EventGraphEditorApp> AppPin = App.Pin();
	
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Fill)
		[
			AppPin->GetGraphEditor().ToSharedRef()
		];
}

FText EventGraphMainTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString("Event Graph Viewer");
}
