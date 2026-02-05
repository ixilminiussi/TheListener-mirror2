// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "DialogueGraphLeaveNode.h"

void UDialogueGraphLeaveNode::AllocateDefaultPins()
{
	CreateCustomPin(EGPD_Input, FGuid::NewGuid(), TEXT("DialoguePin"));
}

void UDialogueGraphLeaveNode::GetNodeContextMenuActions(class UToolMenu* Menu,
                                                        class UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);
}

FLinearColor UDialogueGraphLeaveNode::GetNodeColor() const
{
	return FLinearColor(FColor::Purple);
}

FString UDialogueGraphLeaveNode::GetNodeTitle() const
{
	return FString("Leave Node");
}

void UDialogueGraphLeaveNode::CreateData()
{
	Data = NewObject<UDialogueLineData>(this);
	Data->Type = LineType::LeaveNode;
}

void UDialogueGraphLeaveNode::SetData(UObject* Object)
{
	Data = Cast<UDialogueLineData>(Object);
}
