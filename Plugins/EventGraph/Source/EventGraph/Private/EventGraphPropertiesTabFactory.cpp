#include "EventGraphPropertiesTabFactory.h"

#include "AssetViewUtils.h"
#include "EventGraphEditorApp.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"
#include "EventGraphAsset.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "EventGraphDefines.h"

EventGraphPropertiesTabFactory::EventGraphPropertiesTabFactory(TSharedPtr<class EventGraphEditorApp> InApp) : FWorkflowTabFactory(FName(EventGraphTab2),InApp)
{
	App = InApp;
	TabLabel = FText::FromString("Properties");
	ViewMenuDescription = FText::FromString("Event Graph Properties");
	ViewMenuTooltip = FText::FromString("Event Graph Properties ");

	
}

TSharedRef<SWidget> EventGraphPropertiesTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{TSharedPtr<EventGraphEditorApp> AppPin = App.Pin();
	FPropertyEditorModule& PropertyEdModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs;
	{
	DetailsViewArgs.bAllowSearch = false;
	DetailsViewArgs.bHideSelectionTip = true;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bSearchInitialKeyFocus = true;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.NotifyHook = nullptr;
	DetailsViewArgs.bShowOptions = true;
	DetailsViewArgs.bShowModifiedPropertiesOption = false;
	DetailsViewArgs.bShowScrollBar = true;
	}

	TSharedPtr<IDetailsView> DetailsView = PropertyEdModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(AppPin->GetWorkingAsset());

	TSharedPtr<IDetailsView> SelectedNodePropertiesView = PropertyEdModule.CreateDetailView(DetailsViewArgs);
	SelectedNodePropertiesView->SetObject(nullptr);

	App.Pin()->SetSelectedNodePropertyDetails(SelectedNodePropertiesView);

	return SNew(SVerticalBox)
	+ SVerticalBox::Slot()
	.AutoHeight()
	.HAlign(HAlign_Fill)
	[
		DetailsView.ToSharedRef()
	]
		+ SVerticalBox::Slot()
	.FillHeight(1.0f)
	.HAlign(HAlign_Fill)
	[
		SelectedNodePropertiesView.ToSharedRef()
	];
}

FText EventGraphPropertiesTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString("Event Graph Properties");
}
