#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class DialogueGraphAppMode : public FApplicationMode
{
public:
	DialogueGraphAppMode(TSharedPtr<class DialogueGraphEditorApp> InApp);

	void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;

private:
	TWeakPtr<class DialogueGraphEditorApp> App;
	FWorkflowAllowedTabSet TabSet;
};