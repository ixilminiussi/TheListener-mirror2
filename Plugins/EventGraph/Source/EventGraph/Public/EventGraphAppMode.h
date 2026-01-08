#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class EventGraphAppMode : public FApplicationMode
{
	public:
	EventGraphAppMode(TSharedPtr<class EventGraphEditorApp> InApp);

	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;

private:
	TWeakPtr<class EventGraphEditorApp> App;
	FWorkflowAllowedTabSet TabSet;
};
