#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class DialogueGraphMainTabFactory : public FWorkflowTabFactory
{
public:
	DialogueGraphMainTabFactory(TSharedPtr<class DialogueGraphEditorApp> InApp);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<class DialogueGraphEditorApp> App;
};