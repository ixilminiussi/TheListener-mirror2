#if WITH_EDITOR
#include "DialogueEdGraph.h"
#include "DialogueGraphNode.h"


void UDialogueEdGraph::SetStartNode(UDialogueGraphNode* NewStartNode)
{
	UDialogueGraphNode* StartNode = GetStartNode();
	if (StartNode != nullptr)
	{
		StartNode->SetIsStartNode(false);
		NotifyNodeChanged(StartNode);
	}
	StartNodeID = NewStartNode->NodeGuid;
	NewStartNode->SetIsStartNode(true);
	NotifyNodeChanged(NewStartNode);
}

class UDialogueGraphNode* UDialogueEdGraph::GetStartNode()
{
	
	for (UEdGraphNode* Node : Nodes)
	{
		if (Node->NodeGuid == StartNodeID)
		{
			return Cast<UDialogueGraphNode>(Node);
		}
	}
	return nullptr;
}

void UDialogueEdGraph::OnNodeAdded(UDialogueGraphBaseNode* Node)
{
	//Set as Start Node if no start node available
	if ( UDialogueGraphNode* CastNode = Cast<UDialogueGraphNode>(Node))
	{
		if (GetStartNode() == nullptr )
		{
			StartNodeID = CastNode->NodeGuid;
			CastNode->SetIsStartNode(true);
			NotifyNodeChanged(CastNode);
		}
	}
}

void UDialogueEdGraph::OnNodeDestroyed(UDialogueGraphBaseNode* Node)
{
	if (Node->NodeGuid == StartNodeID)
	{
		int LowestInput = std::numeric_limits<int>::max();
		FGuid LowestInputsNode  = FGuid();
		for (UEdGraphNode* CheckNode : Nodes)
		{
			if (UDialogueGraphNode* CastNode = Cast<UDialogueGraphNode>(CheckNode))
			{
				if (CastNode->GetInputsNumber() < LowestInput && CastNode->NodeGuid != StartNodeID)
				{
					LowestInput = CastNode->GetInputsNumber();
					LowestInputsNode = CastNode->NodeGuid;
				}
			}
		}
		StartNodeID = LowestInputsNode;
		if (UDialogueGraphNode* StartNode = GetStartNode())
		{
			NotifyNodeChanged(StartNode);
			StartNode->SetIsStartNode(true);
		}
	}
}

void UDialogueEdGraph::InitializeData()
{
	StartNodeID.Invalidate();
	SetFlags(RF_Transactional);
}
#endif
