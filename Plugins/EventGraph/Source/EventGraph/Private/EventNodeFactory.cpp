#include "EventNodeFactory.h"

#include "EventGraphBaseConditionNode.h"
#include "EventGraphBaseNode.h"
#include "SEventGraphNode.h"

TSharedPtr<SGraphNode> FEventNodeFactory::CreateNode(UEdGraphNode* Node) const
{
	if (UEventGraphBaseConditionNode* MyNode = Cast<UEventGraphBaseConditionNode>(Node))
	{
		return SNew(SEventGraphNode, MyNode);
	}
	return nullptr;
}
