#include "DialogueGraphAppMode.h"
#include "DialogueDefines.h"
#include "DialogueGraphEditorApp.h"
#include "DialogueGraphMainTabFactory.h"

DialogueGraphAppMode::DialogueGraphAppMode(TSharedPtr<class DialogueGraphEditorApp> InApp) : FApplicationMode(DialogueGraphAppModeName), App(InApp)
{
	TabSet.RegisterFactory(MakeShareable(new DialogueGraphMainTabFactory(InApp)));

	TabLayout = FTabManager::NewLayout(TEXT("DialogueGraphAppModeLayout_1"))
	->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewSplitter()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
			FTabManager::NewStack()
						->SetSizeCoefficient(0.8)
						->AddTab(DialogueGraphTab1, ETabState::OpenedTab)
			)
			
		)
	);
}

void DialogueGraphAppMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<DialogueGraphEditorApp> AppPin = App.Pin();
	AppPin->PushTabFactories(TabSet);
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void DialogueGraphAppMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void DialogueGraphAppMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}
