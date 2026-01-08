#include "DialogueGraphMainTabFactory.h"
#include "DialogueGraphEditorApp.h"
#include "DialogueDefines.h"

DialogueGraphMainTabFactory::DialogueGraphMainTabFactory(TSharedPtr<DialogueGraphEditorApp> InApp): FWorkflowTabFactory(FName(DialogueGraphTab1),InApp)
{
	App = InApp;
	TabLabel = FText::FromString("Dialogue Graph");
	ViewMenuDescription = FText::FromString("Dialogue Graph View");
	ViewMenuTooltip = FText::FromString("Dialogue Graph View Tooltip");
}

TSharedRef<SWidget> DialogueGraphMainTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<DialogueGraphEditorApp> AppPin = App.Pin();

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Fill)
		[
			AppPin->GetSlateGraph().ToSharedRef()
		];
}

FText DialogueGraphMainTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString("Dialogue Graph Viewer");
}
