#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class EventGraphEditorApp : public FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook
{
public:
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	void InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InToolkitHost, UObject* inObject);

	class UEventGraphAsset* GetWorkingAsset() {return WorkingAsset;}
	class UEdGraph* GetWorkingGraph() {return WorkingGraph;}
	TSharedPtr<FUICommandList> GetToolkitCommands() {return ToolkitCommands;}
	TSharedPtr<SGraphEditor> GetGraphEditor() {return GraphEditor;}
	
	virtual FName GetToolkitFName() const override {return FName("EventGraphEditorApp");}
	virtual FText GetBaseToolkitName() const override {return FText::FromString("EventGraphEditorApp");}
	virtual FString GetWorldCentricTabPrefix() const override {return FString("EventGraphEditorApp");}
	virtual FLinearColor GetWorldCentricTabColorScale() const override{return FLinearColor(0.6f,0.1f,0.92f);} 
	virtual FString GetDocumentationLink() const override {return FString("https://www.notion.so/Event-graph-Tutorial-28d4a62bf94e806296e8d06fc0eb64d5?source=copy_link");}
	virtual void OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) override {}
	virtual void OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) override {}

	void OnGraphSelectionChanged(const FGraphPanelSelectionSet& Set);

	void SetSelectedNodePropertyDetails(TSharedPtr<IDetailsView> DetailsView);

	UFUNCTION()
	void SetGraphDirty(const FEdGraphEditAction& EditAction);
	UFUNCTION()
	void OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& Event);
	
	virtual void SaveAsset_Execute() override;

	void UpdateGraphFromAsset();
	void UpdateAssetFromGraph();

	void ToggleSelectedNodes();
	bool CanToggleNodes();
	
	void DeleteSelectedNodes();
	bool CanDeleteNodes();

	void CopySelectedNodes();
	bool CanCopySelectedNodes();

	void PasteSelectedNodes();
	bool CanPasteSelectedNodes();

	void CutSelectedNodes();
	bool CanCutSelectedNodes();

	virtual void OnClose() override;
	

private:
	UPROPERTY()
	class UEventGraphAsset* WorkingAsset = nullptr;

	UPROPERTY()
	class UEdGraph* WorkingGraph = nullptr;

	UPROPERTY()
	TSharedPtr<FUICommandList> ToolkitCommands;
	UPROPERTY()
	TSharedPtr<SGraphEditor> GraphEditor;
	UPROPERTY()
	TSharedPtr<IDetailsView> SelectedNodePropertyDetails;

	FDelegateHandle GraphChangeDelegateHandle;

	void BindCommands();
	
};