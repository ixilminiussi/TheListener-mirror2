#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class EventGraphMainTabFactory : public FWorkflowTabFactory
{
	public:
	EventGraphMainTabFactory(TSharedPtr<class EventGraphEditorApp> InApp);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

private:
	TWeakPtr<class EventGraphEditorApp> App;
};