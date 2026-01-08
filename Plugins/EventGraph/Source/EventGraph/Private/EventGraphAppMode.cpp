#include "EventGraphAppMode.h"
#include "EventGraphEditorApp.h"
#include "EventGraphMainTabFactory.h"
#include "EventGraphPropertiesTabFactory.h"
#include "EventGraphDefines.h"

EventGraphAppMode::EventGraphAppMode(TSharedPtr<class EventGraphEditorApp> InApp) : FApplicationMode(TEXT("EventGraphAppMode"))
{
	App = InApp;
	TabSet.RegisterFactory(MakeShareable(new EventGraphMainTabFactory(InApp)));
	TabSet.RegisterFactory(MakeShareable(new EventGraphPropertiesTabFactory(InApp)));

	TabLayout = FTabManager::NewLayout(TEXT("EventGraphAppModeLayout_1"))
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
						->AddTab(EventGraphTab1, ETabState::OpenedTab)
			)
			->Split
			(
			FTabManager::NewStack()
						->SetSizeCoefficient(0.2)
						->AddTab(EventGraphTab2, ETabState::OpenedTab)
			)
			
		)
	);
}

void EventGraphAppMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<EventGraphEditorApp> AppPin = App.Pin();
	AppPin->PushTabFactories(TabSet);
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void EventGraphAppMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void EventGraphAppMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}
