#include "DialogueGraphSchema.h"

void UDialogueGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	TSharedPtr<FNewDialogueNodeAction> NewDialogueNodeAction(new FNewDialogueNodeAction(
			FText::FromString("Dialogue"),
			FText::FromString("Dialogue Node"),
			FText::FromString("Adds a new Dialogue node"),
			0
			));
	TSharedPtr<FNewAutoDialogueNodeAction> NewAutoDialogueNodeAction(new FNewAutoDialogueNodeAction(
			FText::FromString("Dialogue"),
			FText::FromString("Dialogue Auto Node"),
			FText::FromString("Adds a new Auto Dialogue node"),
			0
			));
	TSharedPtr<FNewLeaveDialogueNodeAction> NewDialogueLeaveNodeAction(new FNewLeaveDialogueNodeAction(
			FText::FromString("Dialogue"),
			FText::FromString("Leave Node"),
			FText::FromString("Adds a new Node to leave the Dialogue but not finishing it"),
			0
			));
	ContextMenuBuilder.AddAction(NewDialogueNodeAction);
	ContextMenuBuilder.AddAction(NewAutoDialogueNodeAction);
	ContextMenuBuilder.AddAction(NewDialogueLeaveNodeAction);
}

const FPinConnectionResponse UDialogueGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (A == nullptr || B == nullptr) {return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("One Pin or Both are NULL"));}

	if (A->Direction == B->Direction) {return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Pins has the same direction and should'nt"));}

	if (A->PinType.PinSubCategory != B->PinType.PinSubCategory) {return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Pins are not the same type"));}
	ECanCreateConnectionResponse Response = A->Direction == EGPD_Output ? CONNECT_RESPONSE_BREAK_OTHERS_A : CONNECT_RESPONSE_BREAK_OTHERS_B;
	return FPinConnectionResponse(Response, TEXT("Looks good to me "));
}

UEdGraphNode* FNewDialogueNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, bool bSelectNewNode)
{
	const FScopedTransaction Transaction(NSLOCTEXT("MyGraph", "AddNode", "Add Dialogue Node"));
	
	ParentGraph->Modify();
	
	UDialogueGraphNode* NewNode = NewObject<UDialogueGraphNode>(ParentGraph,
		UDialogueGraphNode::StaticClass(),
		NAME_None,
		RF_Transactional);
	NewNode->Modify();

	NewNode->Setup(ParentGraph, FromPin, Location, bSelectNewNode);
	NewNode->CreateData();

	return NewNode;
}

UEdGraphNode* FNewLeaveDialogueNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, bool bSelectNewNode)
{
	const FScopedTransaction Transaction(NSLOCTEXT("MyGraph", "AddNode", "Add Leave Node"));
	
	ParentGraph->Modify();
	
	UDialogueGraphLeaveNode* NewNode = NewObject<UDialogueGraphLeaveNode>(ParentGraph,
		UDialogueGraphLeaveNode::StaticClass(),
		NAME_None,
		RF_Transactional);
	NewNode->Modify();

	NewNode->Setup(ParentGraph, FromPin, Location, bSelectNewNode);
	NewNode->CreateData();

	return NewNode;
}

UEdGraphNode* FNewAutoDialogueNodeAction::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, bool bSelectNewNode)
{
	const FScopedTransaction Transaction(NSLOCTEXT("MyGraph", "AddNode", "Add Auto Dialogue Node"));
	
	ParentGraph->Modify();
	
	UDialogueGraphNode* NewNode = NewObject<UDialogueGraphNode>(ParentGraph,
		UDialogueGraphNode::StaticClass(),
		NAME_None,
		RF_Transactional);
	NewNode->Modify();

	NewNode->Setup(ParentGraph, FromPin, Location, bSelectNewNode);
	NewNode->CreateData();
	NewNode->ToggleAuto();

	return NewNode;
}
