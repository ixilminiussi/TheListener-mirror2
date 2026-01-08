#include "EventGraphSchema.h"
#include "EventGraphBaseConditionNode.h"
#include "EventGraphBaseNode.h"
#include "EventGraphConditionData.h"
#include "EventGraphEventConditionNode.h"
#include "EventGraphLogicNode.h"

void UEventGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	//Event Nodes
	TSharedPtr<FNewEventNodeAction> NewEventConditionNodeAction(new FNewEventNodeAction(
			FText::FromString("Event"),
			FText::FromString("Event Node"),
			FText::FromString("Adds a new condition node"),
			0
			));
	ContextMenuBuilder.AddAction(NewEventConditionNodeAction);

	//Condition Nodes
	for(TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;
		if(It->IsChildOf(UEventGraphBaseConditionData::StaticClass()) && !It->HasAnyClassFlags(CLASS_Abstract) && !It->GetName().StartsWith(TEXT("SKEL_")) && !It->GetName().StartsWith(TEXT("REINST_")))
		{
			UE_LOG(LogTemp, Display, TEXT("%s"), *It->GetName());
			TSharedPtr<FNewConditionNodeAction> NewConditionNodeAction2(new FNewConditionNodeAction(
			FText::FromString("Condition Nodes"),
			Class->GetDisplayNameText(),
			FText::FromString("Adds a new condition node"),
			0,
			Class
			));
			ContextMenuBuilder.AddAction(NewConditionNodeAction2);
		}
	}

	//Logic Conditions
	for(TObjectIterator<UClass> It; It; ++It)
	{
		UClass* Class = *It;
		if(It->IsChildOf(UEventGraphLogicConditionData::StaticClass()) && !It->HasAnyClassFlags(CLASS_Abstract))
		{
			TSharedPtr<FNewLogicConditionNodeAction> NewConditionNodeAction2(new FNewLogicConditionNodeAction(
			FText::FromString("Logic Nodes"),
			Class->GetDisplayNameText(),
			FText::FromString("Adds a new condition node"),
			0,
			Class
			));
			ContextMenuBuilder.AddAction(NewConditionNodeAction2);
		}
	}

}

const FPinConnectionResponse UEventGraphSchema::CanCreateConnection(const UEdGraphPin* InPinA, const UEdGraphPin* InPinB) const 
{
	if (InPinA == nullptr || InPinB == nullptr) {return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("One Pin or Both are NULL"));}

	if (InPinA->Direction == InPinB->Direction) {return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Pins has the same direction and should'nt"));}

	if (InPinA->PinType.PinSubCategory != InPinB->PinType.PinSubCategory) {return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Pins are not the same type"));}
	ECanCreateConnectionResponse Response = InPinA->Direction == EGPD_Input ? CONNECT_RESPONSE_BREAK_OTHERS_A : CONNECT_RESPONSE_BREAK_OTHERS_B;
	return FPinConnectionResponse(Response, TEXT("Looks good to me "));
}

UEdGraphNode* FNewConditionNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location,
                                            bool bSelectNewNode)
{
	// Start a transaction so Undo/Redo will capture this
	const FScopedTransaction Transaction(NSLOCTEXT("MyGraph", "AddNode", "Add Condition Node"));
	
	ParentGraph->Modify();
	
	UEventGraphBaseConditionNode* NewNode = NewObject<UEventGraphBaseConditionNode>(ParentGraph,
		UEventGraphBaseConditionNode::StaticClass(),
		NAME_None,
		RF_Transactional);
	NewNode->Modify();

	NewNode->Setup(ParentGraph, FromPin, Location, bSelectNewNode);
	NewNode->CreateData(CurrentClass);

	return NewNode;
}

UEdGraphNode* FNewEventNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location,
	bool bSelectNewNode)
{
	// Start a transaction so Undo/Redo will capture this
	const FScopedTransaction Transaction(NSLOCTEXT("MyGraph", "AddNode", "Add Event Node"));
	
	ParentGraph->Modify();
	
	UEventGraphEventConditionNode* NewNode = NewObject<UEventGraphEventConditionNode>(ParentGraph,
		UEventGraphEventConditionNode::StaticClass(),
		NAME_None,
		RF_Transactional);
	NewNode->Modify();

	NewNode->CreateData(UEventGraphEventConditionData::StaticClass());
	NewNode->Setup(ParentGraph, FromPin, Location, bSelectNewNode);
	

	return NewNode;
}

UEdGraphNode* FNewLogicConditionNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, bool bSelectNewNode)
{
	// Start a transaction so Undo/Redo will capture this
	const FScopedTransaction Transaction(NSLOCTEXT("MyGraph", "AddNode", "Add Logic Node"));
	
	ParentGraph->Modify();
	
	UEventGraphLogicNode* NewNode = NewObject<UEventGraphLogicNode>(ParentGraph,
		UEventGraphLogicNode::StaticClass(),
		NAME_None,
		RF_Transactional);
	NewNode->Modify();

	NewNode->CreateData(CurrentClass);
	NewNode->Setup(ParentGraph, FromPin, Location, bSelectNewNode);
	

	return NewNode;
}

