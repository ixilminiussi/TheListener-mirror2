#include "EventGraphBaseNode.h"
#include "Framework/Commands/UIAction.h"
#include "ToolMenu.h"
#include "EventGraphDefines.h"
#include "EventGraphSchema.h"

void UEventGraphBaseNode::Setup(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	CreateNewGuid();
	PostPlacedNewNode();
	AllocateDefaultPins();
	NodePosX = Location.X;
	NodePosY = Location.Y;
	
	ParentGraph->AddNode(this,true,bSelectNewNode);
	
	if (FromPin != nullptr)
	{
		AutowireNewNode(FromPin);
	}
}

void UEventGraphBaseNode::AllocateDefaultPins()
{
	ConditionOutputPin = CreateCustomPin(EGPD_Output, TEXT(""));
	Super::AllocateDefaultPins();
}

bool UEventGraphBaseNode::Modify(bool bAlwaysMarkDirty)
{
	return Super::Modify(bAlwaysMarkDirty);
}

void UEventGraphBaseNode::GetNodeContextMenuActions(class UToolMenu* Menu,
                                                    class UGraphNodeContextMenuContext* Context) const
{
	/*
	*/
}

void UEventGraphBaseNode::PostEditUndo()
{
	Super::PostEditUndo();
	
	if (Pins.Num() == 0)
	{
		AllocateDefaultPins();
	}

	GetGraph()->NotifyGraphChanged();
}

void UEventGraphBaseNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	if (FromPin == nullptr){return;}
	
	UEdGraphPin* TargetLinkPin = nullptr;
	if (FromPin->Direction == EGPD_Input)
	{
		if (GetSchema()->ArePinsCompatible(FromPin,ConditionOutputPin.Get(),nullptr))
		{
			TargetLinkPin = ConditionOutputPin.Get();
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

FEdGraphPinReference UEventGraphBaseNode::AddInputPins()
{
	UEdGraphPin* NewPin = CreateCustomPin(EGPD_Input, TEXT(""));
	InputPins.Add(NewPin);
	
	GetGraph()->NotifyGraphChanged();
	GetGraph()->Modify();
	return NewPin;
}

FEdGraphPinReference UEventGraphBaseNode::AddInputPinsID(FGuid NewID)
{
	UEdGraphPin* NewPin = CreateCustomPin(EGPD_Input, TEXT(""));
	NewPin->PinId = NewID;
	InputPins.Add(NewPin);
	
	GetGraph()->NotifyGraphChanged();
	GetGraph()->Modify();
	return NewPin;
}

void UEventGraphBaseNode::RemoveInputPins()
{
	if (InputPins.Num() > 2)
	{
		UEdGraphPin* ToRemove = InputPins.Last().Get();
		InputPins.Remove(ToRemove);
		RemovePin(ToRemove);
		

		GetGraph()->NotifyGraphChanged();
		GetGraph()->Modify();
	}
}

void UEventGraphBaseNode::RemoveAllInputPins()
{
	for (FEdGraphPinReference Pin : InputPins)
	{
		UEdGraphPin* ToRemove = Pin.Get();
		RemovePin(ToRemove);
	}
	InputPins.Empty();
}

void UEventGraphBaseNode::ChangeOutputPinsID(FGuid NewID)
{
	UEdGraphPin* PinNewID = ConditionOutputPin.Get();
	PinNewID->PinId = NewID;
	//Update New ID
	ConditionOutputPin = PinNewID;
}

void UEventGraphBaseNode::ReloadPins()
{
	InputPins.Empty();
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin->Direction == EGPD_Output)
		{
			ConditionOutputPin.SetPin(Pin);
		}
		else
		{
			InputPins.Add(Pin);
		}
	}
}


UEdGraphPin* UEventGraphBaseNode::CreateCustomPin(EEdGraphPinDirection Direction, FName Name)
{
	FName Category = (Direction == EGPD_Input ? TEXT("Inputs") : TEXT("Outputs"));
	FName SubCategory = TEXT("ConditionPin");

	UEdGraphPin* Pin = CreatePin(
		Direction,
		Category,
		Name
	);
	Pin->PinType.PinSubCategory = SubCategory;
	return Pin;
}
