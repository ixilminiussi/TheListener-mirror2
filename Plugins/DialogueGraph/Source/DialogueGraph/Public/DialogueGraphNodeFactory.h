#pragma once
#include "DialogueGraphLeaveNode.h"
#include "DialogueGraphNode.h"
#include "EdGraphUtilities.h"
#include "SDialogueGraphNode.h"
#include "SLeaveNode.h"

class FDialogueNodeFactory : public FGraphPanelNodeFactory
{
public:
	virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* Node) const override
	{
		if (UDialogueGraphNode* MyNode = Cast<UDialogueGraphNode>(Node))
		{
			return SNew(SDialogueGraphNode, MyNode);
		}
		else if (UDialogueGraphLeaveNode* LeaveNode = Cast<UDialogueGraphLeaveNode>(Node))
		{
			return SNew(SLeaveNode, LeaveNode);
		}
		return nullptr;
	}
};
