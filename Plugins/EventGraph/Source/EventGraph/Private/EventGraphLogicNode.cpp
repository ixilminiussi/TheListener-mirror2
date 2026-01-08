// Fill out your copyright notice in the Description page of Project Settings.


#include "EventGraphLogicNode.h"
#include "EventGraphDefines.h"
#include "EventGraphConditionData.h"

void UEventGraphLogicNode::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	AddInputPins();

	UEventGraphLogicConditionData* LogicData =  Cast<UEventGraphLogicConditionData>(GetData());
	if (LogicData == nullptr ){return;}
	if (!LogicData->CanHaveMultipleInputPins() ){return;}
	AddInputPins();
}

void UEventGraphLogicNode::GetNodeContextMenuActions(class UToolMenu* Menu,
	class UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);
	UEventGraphLogicConditionData* LogicData =  Cast<UEventGraphLogicConditionData>(GetData());
	if (LogicData == nullptr ){return;}
	if (!LogicData->CanHaveMultipleInputPins() ){return;}
	FToolMenuSection& Section = Menu->AddSection(TEXT("Pin Actions"),FText::FromString(TEXT("Logic Node Actions")));

	UEventGraphLogicNode* Node = (UEventGraphLogicNode*)this;
	Section.AddMenuEntry(
		TEXT("AddPinEntry"),
		FText::FromString(TEXT("Add Pin")),
		FText::FromString(TEXT("Create A New Pin")),
		FSlateIcon(EventGraphEditorStyle, TEXT("AddPin.EventGraphAsset")),
		FUIAction(FExecuteAction::CreateLambda(
		[Node](){
					Node->AddInputPins();
				}
			))
		);

	Section.AddMenuEntry(
		TEXT("RemovePinEntry"),
		FText::FromString(TEXT("Remove Pin")),
		FText::FromString(TEXT("Remove a Pin")),
		FSlateIcon(EventGraphEditorStyle, TEXT("RemovePin.EventGraphAsset")),
		FUIAction(FExecuteAction::CreateLambda(
		[Node](){
					Node->RemoveInputPins();
				}
			))
		);
}
