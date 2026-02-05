#include "DialogueGraphEditorApp.h"
#include "DialogueDefines.h"
#include "DialogueEdGraph.h"
#include "DialogueGraphAppMode.h"
#include "DialogueGraphAsset.h"
#include "DialogueGraphData.h"
#include "DialogueGraphSchema.h"
#include "EdGraphUtilities.h"
#include "GraphEditorActions.h"
#include "SNodePanel.h"
#include "Chaos/Deformable/MuscleActivationConstraints.h"
#include "Framework/Commands/GenericCommands.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "WorldPartition/DataLayer/DataLayerType.h"

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
	Asset->OnPreSave.AddRaw(this,&DialogueGraphEditorApp::SaveAsset_Execute);

	Graph = Cast<UDialogueEdGraph>(FBlueprintEditorUtils::CreateNewGraph(
		Asset,
		NAME_None,
		UDialogueEdGraph::StaticClass(),
		UDialogueGraphSchema::StaticClass()
	));
	Graph->InitializeData();
	

	SGraphEditor::FGraphEditorEvents GraphEvents;
	GraphEvents.OnSelectionChanged.BindRaw(this, &DialogueGraphEditorApp::OnGraphSelectionChanged);
	
	SlateGraph = SNew(SGraphEditor)
		.IsEnabled(true)
		.GraphToEdit(GetGraph())
		.AdditionalCommands(GetToolkitCommand())
		.GraphEvents(GraphEvents);
	
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

void DialogueGraphEditorApp::OnClose()
{
	FWorkflowCentricApplication::OnClose();
	Asset->OnPreSave.RemoveAll(this);
}

void DialogueGraphEditorApp::OnGraphSelectionChanged(const FGraphPanelSelectionSet& Set)
{
	TArray<UObject*> SelectedNodeProperties;
	for (UObject* Selected : Set)
	{
		if (UDialogueGraphNode* NodeSelected = Cast<UDialogueGraphNode>(Selected))
		{
			SelectedNodeProperties.Add(NodeSelected->GetData());
		}
	}
	if (SelectedNodeProperties.Num() == 0) {PropertyView->SetObject(nullptr);}
	//PropertyView->SetObjects(SelectedNodeProperties);
}

void DialogueGraphEditorApp::RefreshNodeVariables(const FPropertyChangedEvent&)
{
	if (SlateGraph.IsValid())
	{
		SlateGraph->NotifyGraphChanged();
	}
}


void DialogueGraphEditorApp::SetSelecedNodePropertyView(TSharedPtr<IDetailsView> Details)
{
	PropertyView = Details;
	PropertyView->OnFinishedChangingProperties().AddRaw(this, &DialogueGraphEditorApp::RefreshNodeVariables);
}

void DialogueGraphEditorApp::SaveAsset_Execute()
{
	SaveGraph();
	FWorkflowCentricApplication::SaveAsset_Execute();
}

void DialogueGraphEditorApp::LoadGraph()
{
	if (Asset == nullptr || Asset->Graph == nullptr ) {return;}
	Graph->Modify();
	TMap<FGuid,UEdGraphPin*> PinIndex;
	TMap<FGuid,FGuid> Connections;
	
	for (UDialogueGraphNodeRuntime* RuntimeNode : Asset->Graph->Nodes)
	{
		
		UDialogueGraphBaseNode* NewNode = NewObject<UDialogueGraphBaseNode>(Graph,
			RuntimeNode->NodeClass,
			NAME_None,
			RF_Transactional);
		NewNode->Modify();
		FVector2D NodeLocation = FVector2D(RuntimeNode->Position);
		NewNode->Setup(Graph, nullptr, NodeLocation, false, false);
		for (UDialogueGraphPinRuntime* RuntimePin : RuntimeNode->InputPins)
		{
			UEdGraphPin* Pin = NewNode->CreateCustomPin(EGPD_Input,RuntimePin->PinGuid,RuntimePin->PinCategory);
			Pin->PinName = RuntimePin->PinName;
			PinIndex.Add(RuntimePin->PinGuid,Pin);
		}
		for (UDialogueGraphPinRuntime* RuntimePin : RuntimeNode->OutputPins)
		{
			UEdGraphPin* Pin = NewNode->CreateCustomPin(EGPD_Output,RuntimePin->PinGuid,RuntimePin->PinCategory);
			Pin->PinName = RuntimePin->PinName;
			PinIndex.Add(RuntimePin->PinGuid,Pin);
			for (UDialogueGraphPinRuntime* LinkPin : RuntimePin->Connection)
			{
				Connections.Add(RuntimePin->PinGuid,LinkPin->PinGuid);
			}
		}
		NewNode->SetData(RuntimeNode->NodeData);
	}
	for (const TPair<FGuid,FGuid>& Connection : Connections)
	{
		UEdGraphPin* PinFrom = PinIndex[Connection.Key];
		UEdGraphPin* PinTo = PinIndex[Connection.Value];
		
		Graph->GetSchema()->TryCreateConnection(PinFrom,PinTo);
		PinFrom->GetOwningNode()->NodeConnectionListChanged();
		PinTo->GetOwningNode()->NodeConnectionListChanged();
	}
	Graph->NotifyGraphChanged();
}

void DialogueGraphEditorApp::SaveGraph()
{
	TMap<FGuid,UDialogueGraphPinRuntime*> PinIndex;
	TMap<FGuid,FGuid> PinConnections;
	
	Asset->LineDatas.Empty();
	Asset->ConditionDatas.Empty();
	Asset->Start = nullptr;
	Asset->Graph = nullptr;
	
	UDialogueGraphData* GraphData = NewObject<UDialogueGraphData>(Asset);
	
	for (UEdGraphNode* Node : Graph->Nodes)
	{
		UDialogueGraphBaseNode* BaseNode = Cast<UDialogueGraphBaseNode>(Node);
		UDialogueGraphNodeRuntime* RuntimeNode = NewObject<UDialogueGraphNodeRuntime>(GraphData);
		RuntimeNode->Position = BaseNode->GetPosition();
		
		UClass* DataClass = BaseNode->GetDataClass();
		UObject* Data = DuplicateObject(BaseNode->GetData(),RuntimeNode);
		RuntimeNode->NodeClass = BaseNode->GetClass();
		RuntimeNode->NodeData = Data;
		BaseNode->SaveDataAdditional(RuntimeNode);
		
		for (FEdGraphPinReference InputPinRef : BaseNode->GetInputPins())
		{
			UDialogueGraphPinRuntime* RuntimePin = NewObject<UDialogueGraphPinRuntime>(RuntimeNode);
			UEdGraphPin* Pin = InputPinRef.Get();
			RuntimePin->PinCategory = Pin->PinType.PinSubCategory;
			RuntimePin->PinGuid = Pin->PinId;
			RuntimePin->PinName = Pin->PinName;
			PinIndex.Add(RuntimePin->PinGuid,RuntimePin);
			
			RuntimeNode->InputPins.Add(RuntimePin);
		}
		for (FEdGraphPinReference OutputPinRef : BaseNode->GetOutputPins())
		{
			UDialogueGraphPinRuntime* RuntimePin = NewObject<UDialogueGraphPinRuntime>(RuntimeNode);
			UEdGraphPin* Pin = OutputPinRef.Get();
			RuntimePin->PinCategory = Pin->PinType.PinSubCategory;
			RuntimePin->PinGuid = Pin->PinId;
			RuntimePin->PinName = Pin->PinName;
			PinIndex.Add(RuntimePin->PinGuid,RuntimePin);
			
			RuntimeNode->OutputPins.Add(RuntimePin);
			
			//Only need the output links (as it gives the input)
			for (UEdGraphPin* PinLink : Pin->LinkedTo)
			{
				PinConnections.Add(Pin->PinId,PinLink->PinId);
			}
		}
		GraphData->Nodes.Add(RuntimeNode);
	}
	for (UDialogueGraphNodeRuntime* Node : GraphData->Nodes)
	{
		if (UDialogueLineData* DialogueData = Cast<UDialogueLineData>(Node->NodeData))
		{
			check(DialogueData != nullptr);
			Asset->LineDatas.Add(DialogueData);
			for (UAnswer* Answer : DialogueData->Next)
			{
				if (PinIndex.Contains(Answer->PinGuid))
				{
					UDialogueGraphPinRuntime* LinkedPin = PinIndex[Answer->PinGuid];
					UDialogueGraphNodeRuntime* LinkedNode = Cast<UDialogueGraphNodeRuntime>(LinkedPin->GetOuter());
					check(LinkedNode != nullptr);
					Answer->NextLine = Cast<UDialogueLineData>(LinkedNode->NodeData);
					continue;
				}
				Answer->NextLine = nullptr;
			}
			if (DialogueData->IsStart)
			{
				Asset->Start = DialogueData;
			}
		}
	}
	for (const TPair<FGuid,FGuid>& Connection : PinConnections)
	{
		UDialogueGraphPinRuntime* PinFrom = PinIndex[Connection.Key];
		UDialogueGraphPinRuntime* PinTo = PinIndex[Connection.Value];
		
		PinFrom->Connection.Add(PinTo);
	}
	Asset->Graph = GraphData;
	Asset->Modify();
	Asset->MarkPackageDirty();
	UE_LOG(LogTemp, Warning, TEXT("Dirty: %d"), Asset->GetOutermost()->IsDirty());
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
				if (UDialogueGraphBaseNode* CastNode = Cast<UDialogueGraphBaseNode>(Node))
				{
					GetGraph()->OnNodeDestroyed(CastNode);
				}
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

void DialogueGraphEditorApp::SetNewStartNode(class UDialogueGraphNode* Node)
{
	for (UEdGraphNode* Node : GetGraph()->Nodes)
	{
		if (UDialogueGraphNode* DNode = Cast<UDialogueGraphNode>(Node))
		{
			DNode->SetIsStartNode(false);
		}
	}
	Node->SetIsStartNode(true);
	StartNode = Node;
	Graph->NotifyGraphChanged();
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
