#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class DialogueGraphEditorApp : public FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook
{
public:
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	void Init(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InToolKitHost, UObject* inObject);

	class UDialogueGraphAsset* GetAsset() const {return Asset;}
	class UEdGraph* GetGraph() const {return Graph;}
	TSharedPtr<FUICommandList> GetToolkitCommand() const {return ToolkitCommands;}
	TSharedPtr<SGraphEditor> GetSlateGraph() const {return SlateGraph;}

	virtual FName GetToolkitFName() const override {return FName("DialogueGraphEditorApp");}
	virtual FText GetBaseToolkitName() const override {return FText::FromString("DialogueGraphEditorApp");}
	virtual FString GetWorldCentricTabPrefix() const override {return FString("DialogueGraphEditorApp");}
	virtual FLinearColor GetWorldCentricTabColorScale() const override{return FLinearColor(FColor::Green);} 
	virtual FString GetDocumentationLink() const override {return FString("https://www.notion.so/Dialogue-graph-tool-Tutorial-2b64a62bf94e80c9a7b6f51947eab643?source=copy_link");}
	virtual void OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) override {}
	virtual void OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) override {}

	virtual void SaveAsset_Execute() override;

	void LoadGraph();
	void SaveGraph();

	void DeleteSelectedNodes();
	bool CanDeleteNodes();

	void CopySelectedNodes();
	bool CanCopySelectedNodes();

	void PasteSelectedNodes();
	bool CanPasteSelectedNodes();

	void CutSelectedNodes();
	bool CanCutSelectedNodes();

private:
	UPROPERTY()
	class UDialogueGraphAsset* Asset;

	UPROPERTY()
	class UEdGraph* Graph;

	UPROPERTY()
	TSharedPtr<FUICommandList> ToolkitCommand;
	UPROPERTY()
	TSharedPtr<SGraphEditor> SlateGraph;

	void BindCommands();

};


