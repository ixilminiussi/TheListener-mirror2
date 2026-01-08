#include "EventGraphBaseConditionNode.h"

#include "EventGraphDefines.h"
#include "EventGraphConditionData.h"


void UEventGraphBaseConditionNode::GetNodeContextMenuActions(class UToolMenu* Menu,
	class UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);

	if (Data == nullptr ){return;}
	FToolMenuSection& Section = Menu->AddSection(TEXT("Pin Actions"),FText::FromString(TEXT("Node Actions")));
	UEventGraphConditionData* CondData = GetData();
	
	
	Section.AddMenuEntry(
		TEXT("ToggleCondition"),
		FText::FromString(TEXT("Toggle Condition")),
		FText::FromString(TEXT("Toggle the condition between Active and Inactive")),
		FSlateIcon(EventGraphEditorStyle, TEXT("ToggleNode.EventGraphAsset")),
		FUIAction(FExecuteAction::CreateLambda(
		[CondData,this](){
					
					CondData->IsConditionEnabled = !CondData->IsConditionEnabled;
					this->GetGraph()->NotifyNodeChanged(this);
				}
			))
		);
}

void UEventGraphBaseConditionNode::CreateData(UClass* DataClass)
{
	if (DataClass->IsChildOf(UEventGraphConditionData::StaticClass()))
	{
		Data = NewObject<UEventGraphConditionData>(
			this,
			DataClass,
			NAME_None,
			RF_Transactional);
		Data->OnPropertyChanged.AddUObject(this, &UEventGraphBaseConditionNode::UpdateNode);
	}
	
}

void UEventGraphBaseConditionNode::UpdateNode()
{
	//GetGraph()->NotifyNodeChanged(this);
}
