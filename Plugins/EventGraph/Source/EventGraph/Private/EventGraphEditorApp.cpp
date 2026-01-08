#include "EventGraphEditorApp.h"

#include "EdGraphUtilities.h"
#include "EventGraphAppMode.h"
#include "EventGraphAsset.h"
#include "EventGraphBaseConditionNode.h"
#include "EventGraphCommands.h"
#include "EventGraphEventConditionNode.h"
#include "EventGraphLogicNode.h"
#include "EventGraphSchema.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EventGraphSchema.h"
#include "GraphEditorActions.h"
#include "SNodePanel.h"
#include "Framework/Commands/GenericCommands.h"
#include "Runtime/Renderer/Internal/SceneTextures.h"
#include "Windows/WindowsPlatformApplicationMisc.h"
#include "UObject/Object.h"

void EventGraphEditorApp::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

void EventGraphEditorApp::InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InToolkitHost,
	UObject* inObject)
{
	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add(inObject);

	BindCommands();

	WorkingAsset = Cast<UEventGraphAsset>(inObject);
	WorkingAsset->OnPreSave.AddRaw(this,&EventGraphEditorApp::SaveAsset_Execute);

	WorkingGraph = FBlueprintEditorUtils::CreateNewGraph(
		WorkingAsset,
		NAME_None,
		UEdGraph::StaticClass(),
		UEventGraphSchema::StaticClass());

	SGraphEditor::FGraphEditorEvents GraphEvents;
	GraphEvents.OnSelectionChanged.BindRaw(this, &EventGraphEditorApp::OnGraphSelectionChanged);

	GraphEditor = SNew(SGraphEditor)
			.IsEnabled(true)
			.GraphToEdit(GetWorkingGraph())
			.AdditionalCommands(GetToolkitCommands())
			.GraphEvents(GraphEvents);
	
	InitAssetEditor(
		Mode,
		InToolkitHost,
		TEXT("EventGraphEditor"),
		FTabManager::FLayout::NullLayout,
		true,
		true,
		ObjectsToEdit
		);

	AddApplicationMode(TEXT("EventGraphAppMode"),MakeShareable(new EventGraphAppMode(SharedThis(this))));
	SetCurrentMode(TEXT("EventGraphAppMode"));

	UpdateGraphFromAsset();
	
	
	//GraphChangeDelegateHandle = WorkingGraph->AddOnGraphChangedHandler(
	//FOnGraphChanged::FDelegate::CreateSP(this, &EventGraphEditorApp::SetGraphDirty));
}

void EventGraphEditorApp::OnGraphSelectionChanged(const FGraphPanelSelectionSet& Set)
{
	
	TArray<UObject*> SelectedNodeProperties;
	for (UObject* Selected : Set)
	{
		if (UEventGraphBaseConditionNode* NodeSelected = Cast<UEventGraphBaseConditionNode>(Selected))
		{
			SelectedNodeProperties.Add(NodeSelected->GetData());
		}
	}
	if (SelectedNodeProperties.Num() == 0) {SelectedNodePropertyDetails->SetObject(nullptr);}
	SelectedNodePropertyDetails->SetObjects(SelectedNodeProperties);
}

void EventGraphEditorApp::SetSelectedNodePropertyDetails(TSharedPtr<IDetailsView> DetailsView)
{
	SelectedNodePropertyDetails = DetailsView;
	SelectedNodePropertyDetails->OnFinishedChangingProperties().AddRaw(this, &EventGraphEditorApp::OnNodeDetailViewPropertiesUpdated);
}

void EventGraphEditorApp::SetGraphDirty(const FEdGraphEditAction& EditAction)
{
	UPackage* Package = WorkingAsset->GetPackage();
	if (Package)
	{
		Package->MarkPackageDirty();
	}
}

void EventGraphEditorApp::OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& Event)
{
	if (GraphEditor.IsValid())
	{
		GraphEditor->NotifyGraphChanged();
	}
}

void EventGraphEditorApp::SaveAsset_Execute()
{
	UpdateAssetFromGraph();
	FWorkflowCentricApplication::SaveAsset_Execute();
}

void EventGraphEditorApp::UpdateGraphFromAsset()
{
	UE_LOG(LogTemp,Log,TEXT("EventGraphEditorApp::UpdateGraphFromAsset"));
	if (WorkingAsset == nullptr || WorkingAsset->GraphData == nullptr ) {return;}
	WorkingGraph->Modify();
	TMap<FGuid, UEdGraphPin*> PinsMap;
	TMap<FGuid,FGuid> Connections;
	for (UEventGraphNodeRuntime* RuntimeNode : WorkingAsset->GraphData->Nodes)
	{
		UClass* NodeClass = nullptr;
		switch (RuntimeNode->Data->GetType())
		{
		case Logic:
			NodeClass = UEventGraphLogicNode::StaticClass();
			break;
		case Event:
			NodeClass = UEventGraphEventConditionNode::StaticClass();
			break;
		case Condition:
			NodeClass = UEventGraphBaseConditionNode::StaticClass();
			break;
		default:
			break;
		}
		
		UEventGraphBaseConditionNode* NewNode = NewObject<UEventGraphBaseConditionNode>(WorkingGraph,
			NodeClass,
			NAME_None,
			RF_Transactional);
		NewNode->Modify();
		FVector2D Location = FVector2D(RuntimeNode->Position.X,RuntimeNode->Position.Y);
		NewNode->Setup(WorkingGraph,nullptr,Location,false);
		UEventGraphConditionData* NodeData = DuplicateObject<UEventGraphConditionData>(RuntimeNode->Data,NewNode);
		NewNode->SetNewData(NodeData);
		NewNode->RemoveAllInputPins();
		for (UEventGraphPinRuntime* RuntimePin : RuntimeNode->ConditionInputPins)
		{
			UEdGraphPin* NewPin = NewNode->AddInputPinsID(RuntimePin->PinGuid).Get();
			NewPin->PinName = RuntimePin->PinName;
			PinsMap.Add(NewPin->PinId,NewPin);

			//Get The Connections
			for (UEventGraphPinRuntime* PinLinked : RuntimePin->Connection)
			{
				Connections.Add(RuntimePin->PinGuid,PinLinked->PinGuid);
			}
		}
		if (RuntimeNode->ConditionOutputPin != nullptr)
		{
			NewNode->ChangeOutputPinsID(RuntimeNode->ConditionOutputPin->PinGuid);
			UEdGraphPin* OutputPin = NewNode->GetOutputPins().Get();
			OutputPin->PinName = RuntimeNode->ConditionOutputPin->PinName;
			PinsMap.Add(OutputPin->PinId,OutputPin);
		}
	}
	for (TPair<FGuid,FGuid> Link : Connections)
	{
		UEdGraphPin* FromPin = PinsMap[Link.Key];
		UEdGraphPin* ToPin = PinsMap[Link.Value];
		WorkingGraph->GetSchema()->TryCreateConnection(FromPin,ToPin);
		FromPin->GetOwningNode()->NodeConnectionListChanged();
		ToPin->GetOwningNode()->NodeConnectionListChanged();
	}

	
}

void EventGraphEditorApp::UpdateAssetFromGraph()
{
	if (WorkingAsset == nullptr  || WorkingGraph == nullptr) {return;}
	UEventGraphData* NewGraphData = NewObject<UEventGraphData>(WorkingAsset);
	NewGraphData->Nodes.Empty();
	NewGraphData->Events.Empty();
	TMap<FGuid, UEventGraphPinRuntime*> RuntimePins;
	TMap<FGuid,FGuid> Connections;
	for (UEdGraphNode* Node : WorkingGraph->Nodes)
	{
		UEventGraphBaseConditionNode* CastNode = Cast<UEventGraphBaseConditionNode>(Node);
		if (CastNode == nullptr) {UE_LOG(LogTemp,Log,TEXT("Node is not a EventGraphBaseConditionNode")); continue;}
		UEventGraphNodeRuntime* RuntimeNode = NewObject<UEventGraphNodeRuntime>(NewGraphData);
		RuntimeNode->Data = DuplicateObject<UEventGraphConditionData>(CastNode->GetData(),RuntimeNode);
		RuntimeNode->Data->ClearConnectedData();
		RuntimeNode->Position = CastNode->GetPosition();
		
		if (CastNode->GetData()->GetType() == Event)
		{
			NewGraphData->Events.Add(Cast<UEventGraphEventConditionData>(RuntimeNode->Data));
		}

		for (FEdGraphPinReference PinReference : CastNode->GetInputPins())
		{
			UEdGraphPin* Pin = PinReference.Get();
			UEventGraphPinRuntime* RuntimePin = NewObject<UEventGraphPinRuntime>(RuntimeNode);
			RuntimePin->PinName = Pin->PinName;
			RuntimePin->PinGuid = Pin->PinId;

			RuntimeNode->ConditionInputPins.Add(RuntimePin);

			RuntimePins.Add(Pin->PinId, RuntimePin);
			for (UEdGraphPin* LinkedPin : Pin->LinkedTo)
			{
				Connections.Add(Pin->PinId,LinkedPin->PinId);
			}
		}
		UEdGraphPin* OutputPin = CastNode->GetOutputPins().Get();
		if (OutputPin != nullptr)
		{
			UEventGraphPinRuntime* RuntimePin = NewObject<UEventGraphPinRuntime>(RuntimeNode);
			RuntimePin->PinName = OutputPin->PinName;
			RuntimePin->PinGuid = OutputPin->PinId;

			RuntimeNode->ConditionOutputPin = RuntimePin;

			RuntimePins.Add(OutputPin->PinId, RuntimePin);
		}

		NewGraphData->Nodes.Add(RuntimeNode);
	}
	for (TPair<FGuid,FGuid> Connection : Connections)
	{
		UEventGraphPinRuntime* RuntimePinFrom = RuntimePins[Connection.Key];
		UEventGraphPinRuntime* RuntimePinTo = RuntimePins[Connection.Value];
		

		RuntimePinFrom->Connection.Add(RuntimePinTo);
	}
	//Setup Conditions Connections
	for (UEventGraphNodeRuntime* RuntimeNode : NewGraphData->Nodes)
	{
		UEventGraphConditionData* ConditionData = RuntimeNode->Data;
		//Its an Event Node Data
		if (UEventGraphEventConditionData* CastData = Cast<UEventGraphEventConditionData>(ConditionData))
		{
			if (RuntimeNode->ConditionInputPins.Num() < 1) {continue;}
			UEventGraphPinRuntime* RuntimePin = RuntimeNode->ConditionInputPins[0];
			if (RuntimePin != nullptr && RuntimePin->Connection.Num() > 0)
			{
				UEventGraphNodeRuntime* LinkedNode = Cast<UEventGraphNodeRuntime>(RuntimePin->Connection[0]->GetOuter());
				CastData->SetCondition(LinkedNode->Data);
			}
		}
		//Its a logic Node Data
		if (UEventGraphLogicConditionData* CastData = Cast<UEventGraphLogicConditionData>(ConditionData))
		{
			TArray<UEventGraphConditionData*> ConditionsList;
			if (RuntimeNode->ConditionInputPins.Num() < 1) {continue;}
			for (UEventGraphPinRuntime* NodePin : RuntimeNode->ConditionInputPins)
			{
				if (NodePin != nullptr && NodePin->Connection.Num() > 0)
				{
					UEventGraphPinRuntime* LinkedPin = NodePin->Connection[0];
					if (LinkedPin == nullptr ) {continue;}
					UEventGraphNodeRuntime* LinkedNode = Cast<UEventGraphNodeRuntime>(LinkedPin->GetOuter());
					ConditionsList.Add(LinkedNode->Data);
				}
			}
			CastData->SetConditions(ConditionsList);
		}
	}
	WorkingAsset->GraphData = NewGraphData;
	
}

void EventGraphEditorApp::ToggleSelectedNodes()
{
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
	const FScopedTransaction Transaction(NSLOCTEXT("EventGraph", "ToggleNode", "Toggle Node"));

	for (UObject* NodeObj : SelectedNodes)
	{
		if (UEventGraphBaseConditionNode* Node = Cast<UEventGraphBaseConditionNode>(NodeObj))
		{
			if (Node->GetData() != nullptr)
			{
				WorkingGraph->Modify();
				Node->Modify();
				Node->GetData()->IsConditionEnabled = !Node->GetData()->IsConditionEnabled;
				WorkingGraph->NotifyNodeChanged(Node);
			}
		}
	}
}

bool EventGraphEditorApp::CanToggleNodes()
{
	return true;
}

void EventGraphEditorApp::DeleteSelectedNodes()
{
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
	const FScopedTransaction Transaction(NSLOCTEXT("EventGraph", "DeleteNode", "Delete Node"));

	for (UObject* NodeObj : SelectedNodes)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(NodeObj))
		{
			if (Node->CanUserDeleteNode())
			{
				WorkingGraph->Modify();
				Node->Modify();
				WorkingGraph->RemoveNode(Node);
			}
		}
	}

	GraphEditor->ClearSelectionSet();
}

bool EventGraphEditorApp::CanDeleteNodes()
{
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

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

void EventGraphEditorApp::CopySelectedNodes()
{
	
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
	FString CopyString;

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, CopyString);
	FPlatformApplicationMisc::ClipboardCopy(*CopyString);
}

bool EventGraphEditorApp::CanCopySelectedNodes()
{
	return GraphEditor.IsValid() && GraphEditor->GetSelectedNodes().Num() > 0;
}

void EventGraphEditorApp::PasteSelectedNodes()
{
	const FVector2f PasteLocation = GraphEditor->GetPasteLocation2f();
	
	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
	
	const FScopedTransaction Transaction(NSLOCTEXT("EventGraph", "PasteNodes", "Paste Nodes"));
	WorkingGraph->Modify();

	GraphEditor->ClearSelectionSet();

	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(WorkingGraph, ClipboardContent, PastedNodes);

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
		GraphEditor->SetNodeSelection(Node,true);
		Node->CreateNewGuid();
		for (UEdGraphPin* Pin : Node->Pins)
		{
			Pin->PinId = FGuid::NewGuid();
		}
		UEventGraphBaseNode* CastNode = Cast<UEventGraphBaseNode>(Node);
		if (CastNode != nullptr)
		{
			CastNode->ReloadPins();
		}
	}

	WorkingGraph->NotifyGraphChanged();
}

bool EventGraphEditorApp::CanPasteSelectedNodes()
{
	FString ClipboardString;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardString);
	return !ClipboardString.IsEmpty();
}

void EventGraphEditorApp::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedNodes();
}

bool EventGraphEditorApp::CanCutSelectedNodes()
{
	return CanDeleteNodes() && CanCopySelectedNodes();
}

void EventGraphEditorApp::OnClose()
{
	WorkingAsset->OnPreSave.RemoveAll(this);
	FWorkflowCentricApplication::OnClose();
}

void EventGraphEditorApp::BindCommands()
{
	FEventGraphCommands::Register();
	const FGenericCommands& GenericCommands = FGenericCommands::Get();
	const FGraphEditorCommandsImpl& GraphEditorCommands = FGraphEditorCommands::Get();

	ToolkitCommands = MakeShareable(new FUICommandList);
	
	ToolkitCommands->MapAction(
		GenericCommands.Delete,
		FExecuteAction::CreateSP(this, &EventGraphEditorApp::DeleteSelectedNodes),
		FCanExecuteAction::CreateSP(this, &EventGraphEditorApp::CanDeleteNodes)
	);

	ToolkitCommands->MapAction(
		FEventGraphCommands::Get().ToggleNodeAction,
		FExecuteAction::CreateSP(this, &EventGraphEditorApp::ToggleSelectedNodes),
		FCanExecuteAction::CreateSP(this, &EventGraphEditorApp::CanToggleNodes)
	);
	
	ToolkitCommands->MapAction(
		GenericCommands.Copy,
		FExecuteAction::CreateSP(this, &EventGraphEditorApp::CopySelectedNodes),
		FCanExecuteAction::CreateSP(this, &EventGraphEditorApp::CanCopySelectedNodes)
	);

	ToolkitCommands->MapAction(
		GenericCommands.Paste,
		FExecuteAction::CreateSP(this, &EventGraphEditorApp::PasteSelectedNodes),
		FCanExecuteAction::CreateSP(this, &EventGraphEditorApp::CanPasteSelectedNodes)
		);

	ToolkitCommands->MapAction(
		GenericCommands.Cut,
		FExecuteAction::CreateSP(this, &EventGraphEditorApp::CutSelectedNodes),
		FCanExecuteAction::CreateSP(this, &EventGraphEditorApp::CanCutSelectedNodes)
		);

	
	
}
