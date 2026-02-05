// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "DialogueGraphBaseNode.h"

#include "DialogueEdGraph.h"

void UDialogueGraphBaseNode::Setup(class UEdGraph* ParentGraph, class UEdGraphPin* FromPin, const FVector2D Location,
                                   bool bSelectNewNode, bool bIsCreated)
{
	CreateNewGuid();
	
	if (bIsCreated)
	{
		PostPlacedNewNode();
		AllocateDefaultPins();
	}
	
	
	
	NodePosX = Location.X;
	NodePosY = Location.Y;

	ParentGraph->AddNode(this,true,bSelectNewNode);
	if (FromPin)
	{
		AutowireNewNode(FromPin);
	}
}

bool UDialogueGraphBaseNode::Modify(bool bAlwaysMarkDirty)
{
	return Super::Modify(bAlwaysMarkDirty);
}

void UDialogueGraphBaseNode::PostEditUndo()
{
	Super::PostEditUndo();
	
	GetGraph()->NotifyGraphChanged();
}

void UDialogueGraphBaseNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	if (FromPin == nullptr){return;}
	UEdGraphPin* TargetLinkPin = nullptr;
	if (FromPin->Direction == EGPD_Input)
	{
		for (FEdGraphPinReference PinRef : OutputPins)
		{
			UEdGraphPin* Pin = PinRef.Get(); 
			if (GetSchema()->ArePinsCompatible(FromPin,Pin,nullptr))
			{
				TargetLinkPin = Pin;
				break;
			}
		}
	}
	else
	{
		for (FEdGraphPinReference PinRef : InputPins)
		{
			UEdGraphPin* Pin = PinRef.Get(); 
			if (GetSchema()->ArePinsCompatible(FromPin,Pin,nullptr))
			{
				TargetLinkPin = Pin;
				break;
			}
		}
	}

	if (TargetLinkPin != nullptr)
	{
		GetSchema()->TryCreateConnection(FromPin,TargetLinkPin);
		FromPin->GetOwningNode()->NodeConnectionListChanged();
		TargetLinkPin->GetOwningNode()->NodeConnectionListChanged();
	}
}

FLinearColor UDialogueGraphBaseNode::GetNodeColor() const
{
	return FLinearColor::Gray;
}

UEdGraphPin* UDialogueGraphBaseNode::CreateCustomPin(EEdGraphPinDirection Direction, FGuid PinID , FName PinCategory)
{
	Modify(false);
	UEdGraphPin* NewPin = CreatePin(
		Direction,
		(Direction == EGPD_Input ? TEXT("Inputs") : TEXT("Outputs")),
		NAME_None
	);
	
	NewPin->PinType.PinSubCategory = PinCategory;
	NewPin->PinId = PinID;

	switch (Direction)
	{
	case EGPD_Output:
		OutputPins.Add(NewPin);
		break;
	case EGPD_Input:
		InputPins.Add(NewPin);
		break;
	default:
		break;
	}

	return NewPin;
}

void UDialogueGraphBaseNode::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();
	Cast<UDialogueEdGraph>(GetGraph())->OnNodeAdded(this);
}

void UDialogueGraphBaseNode::SaveDataAdditional(UDialogueGraphNodeRuntime* RNode)
{
}
