#include "DialogueGraphMainTabFactory.h"
#include "DialogueGraphEditorApp.h"
#include "DialogueDefines.h"
#include "Styling/SlateStyleRegistry.h"

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
	
	const ISlateStyle* StyleSet = FSlateStyleRegistry::FindSlateStyle(DialogueGraphEditorStyle);
	const FSlateBrush* BGBrush = StyleSet->GetBrush("DialogueGraphAsset.Background");
	
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Fill)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				AppPin->GetSlateGraph().ToSharedRef()
			]
			+ SOverlay::Slot()
			[
				SNew(SImage)
				.Image(BGBrush)
				.ColorAndOpacity(FSlateColor(FColor(255,255,255,150)))
				.Visibility(EVisibility::SelfHitTestInvisible)
			]
		]
		;
}

FText DialogueGraphMainTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString("Dialogue Graph Viewer");
}
