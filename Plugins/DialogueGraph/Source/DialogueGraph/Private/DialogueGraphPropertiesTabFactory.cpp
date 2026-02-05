#include "DialogueGraphPropertiesTabFactory.h"
#include "DialogueDefines.h"
#include "DialogueGraphAsset.h"
#include "DialogueGraphEditorApp.h"

DialogueGraphPropertiesTabFactory::DialogueGraphPropertiesTabFactory(TSharedPtr<DialogueGraphEditorApp> InApp) : FWorkflowTabFactory(FName(DialogueGraphTab2),InApp)
{
	App = InApp;
	TabLabel = FText::FromString("Properties");
	ViewMenuDescription = FText::FromString("Event Graph Properties");
	ViewMenuTooltip = FText::FromString("Event Graph Properties ");
}

TSharedRef<SWidget> DialogueGraphPropertiesTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<DialogueGraphEditorApp> AppPin = App.Pin();
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

	TSharedPtr<IDetailsView> SelectedNodePropertiesView = PropertyEdModule.CreateDetailView(DetailsViewArgs);
	SelectedNodePropertiesView->SetObject(nullptr);

	App.Pin()-> SetSelecedNodePropertyView(SelectedNodePropertiesView);

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString("No worries BOIII ! this thing is temporary"))
			.ColorAndOpacity(FLinearColor(FColor::Silver))
		]
		+ SVerticalBox::Slot()
	.FillHeight(1.0f)
	.HAlign(HAlign_Fill)
	[
		SelectedNodePropertiesView.ToSharedRef()
	];
}

FText DialogueGraphPropertiesTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString("Dialogue Graph Properties");
}
