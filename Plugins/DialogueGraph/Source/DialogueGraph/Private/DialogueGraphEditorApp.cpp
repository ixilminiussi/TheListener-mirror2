#include "DialogueGraphEditorApp.h"
#include "DialogueDefines.h"
#include "DialogueGraphAppMode.h"
#include "DialogueGraphAsset.h"
#include "EdGraphUtilities.h"
#include "GraphEditorActions.h"
#include "SNodePanel.h"
#include "Framework/Commands/GenericCommands.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

void DialogueGraphEditorApp::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

void DialogueGraphEditorApp::Init(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InToolKitHost,
	UObject* inObject)
{
	TArray<UObject*> Objects;
	Objects.Add(inObject);

	BindCommands();

	Asset = Cast<UDialogueGraphAsset>(inObject);

	Graph = FBlueprintEditorUtils::CreateNewGraph(
		Asset,
		NAME_None,
		UEdGraph::StaticClass(),
		UEdGraphSchema::StaticClass()
	);

	SlateGraph = SNew(SGraphEditor)
		.IsEnabled(true)
		.GraphToEdit(GetGraph())
		.AdditionalCommands(GetToolkitCommand());
	
	InitAssetEditor(
		Mode,
		InToolKitHost,
		TEXT("DialogueGraphEditor"),
		FTabManager::FLayout::NullLayout,
		true,
		true,
		Objects);

	AddApplicationMode(DialogueGraphAppModeName,MakeShareable(new DialogueGraphAppMode(SharedThis(this))));
	SetCurrentMode(DialogueGraphAppModeName);

	LoadGraph();
}


void DialogueGraphEditorApp::SaveAsset_Execute()
{
	SaveGraph();
	FWorkflowCentricApplication::SaveAsset_Execute();
}

void DialogueGraphEditorApp::LoadGraph()
{
}

void DialogueGraphEditorApp::SaveGraph()
{
}

void DialogueGraphEditorApp::DeleteSelectedNodes()
{
	const FGraphPanelSelectionSet SelectedNodes = SlateGraph->GetSelectedNodes();
	const FScopedTransaction Transaction(NSLOCTEXT("EventGraph", "DeleteNode", "Delete Node"));

	for (UObject* NodeObj : SelectedNodes)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(NodeObj))
		{
			if (Node->CanUserDeleteNode())
			{
				Graph->Modify();
				Node->Modify();
				Graph->RemoveNode(Node);
			}
		}
	}

	SlateGraph->ClearSelectionSet();
}

bool DialogueGraphEditorApp::CanDeleteNodes()
{
	const FGraphPanelSelectionSet SelectedNodes = SlateGraph->GetSelectedNodes();

	for (UObject* NodeObj : SelectedNodes)
	{
		if (const UEdGraphNode* Node = Cast<UEdGraphNode>(NodeObj))
		{
			if (Node->CanUserDeleteNode())
			{
				return true;
			}
		}
	}

	return false;
}

void DialogueGraphEditorApp::CopySelectedNodes()
{
	const FGraphPanelSelectionSet SelectedNodes = SlateGraph->GetSelectedNodes();
	FString CopyString;

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, CopyString);
	FPlatformApplicationMisc::ClipboardCopy(*CopyString);
}

bool DialogueGraphEditorApp::CanCopySelectedNodes()
{
	return SlateGraph.IsValid() && SlateGraph->GetSelectedNodes().Num() > 0;;
}

void DialogueGraphEditorApp::PasteSelectedNodes()
{
	const FVector2f PasteLocation = SlateGraph->GetPasteLocation2f();
	
	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
	
	const FScopedTransaction Transaction(NSLOCTEXT("EventGraph", "PasteNodes", "Paste Nodes"));
	Graph->Modify();

	SlateGraph->ClearSelectionSet();

	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(Graph, ClipboardContent, PastedNodes);

	if (PastedNodes.Num() == 0)
	{
		return;
	}

	FVector2f AveragePos = FVector2f::Zero();
	for (UEdGraphNode* Node : PastedNodes)
	{
		AveragePos += Node->GetPosition();
	}
	AveragePos /= PastedNodes.Num();
	const FVector2f PasteOffset = PasteLocation - AveragePos;
	for (UEdGraphNode* Node : PastedNodes)
	{
		Node->Modify();
		Node->NodePosX += PasteOffset.X;
		Node->NodePosY += PasteOffset.Y;
		Node->SnapToGrid(SNodePanel::GetSnapGridSize());
		SlateGraph->SetNodeSelection(Node,true);
		Node->CreateNewGuid();
		for (UEdGraphPin* Pin : Node->Pins)
		{
			Pin->PinId = FGuid::NewGuid();
		}
		/* TODO
		UEventGraphBaseNode* CastNode = Cast<UEventGraphBaseNode>(Node);
		if (CastNode != nullptr)
		{
			CastNode->ReloadPins();
		}*/
	}

	Graph->NotifyGraphChanged();
}

bool DialogueGraphEditorApp::CanPasteSelectedNodes()
{
	FString ClipboardString;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardString);
	return !ClipboardString.IsEmpty();
}

void DialogueGraphEditorApp::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedNodes();
}

bool DialogueGraphEditorApp::CanCutSelectedNodes()
{
	return CanCopySelectedNodes() && CanDeleteNodes();
}

void DialogueGraphEditorApp::BindCommands()
{
	const FGenericCommands& GenericCommands = FGenericCommands::Get();
	const FGraphEditorCommandsImpl& GraphEditorCommands = FGraphEditorCommands::Get();

	ToolkitCommands = MakeShareable(new FUICommandList);
	
	ToolkitCommands->MapAction(
		GenericCommands.Delete,
		FExecuteAction::CreateSP(this, &DialogueGraphEditorApp::DeleteSelectedNodes),
		FCanExecuteAction::CreateSP(this, &DialogueGraphEditorApp::CanDeleteNodes)
	);
	
	ToolkitCommands->MapAction(
		GenericCommands.Copy,
		FExecuteAction::CreateSP(this, &DialogueGraphEditorApp::CopySelectedNodes),
		FCanExecuteAction::CreateSP(this, &DialogueGraphEditorApp::CanCopySelectedNodes)
	);

	ToolkitCommands->MapAction(
		GenericCommands.Paste,
		FExecuteAction::CreateSP(this, &DialogueGraphEditorApp::PasteSelectedNodes),
		FCanExecuteAction::CreateSP(this, &DialogueGraphEditorApp::CanPasteSelectedNodes)
		);

	ToolkitCommands->MapAction(
		GenericCommands.Cut,
		FExecuteAction::CreateSP(this, &DialogueGraphEditorApp::CutSelectedNodes),
		FCanExecuteAction::CreateSP(this, &DialogueGraphEditorApp::CanCutSelectedNodes)
		);
}
