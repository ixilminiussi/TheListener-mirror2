#pragma once

#include "EventGraphBaseNode.h"
#include "EventGraphConditionData.h"
#include "EventGraphBaseConditionNode.generated.h"

UCLASS(Blueprintable)
class EVENTGRAPH_API UEventGraphBaseConditionNode : public UEventGraphBaseNode
{
	GENERATED_BODY()

public:

	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override {return Data != nullptr ? Data->GetNodeTitle() : FText::FromString("Condition Node");}
	virtual FLinearColor GetNodeTitleColor() const override {return Data != nullptr ? Data->GetNodeColor() :  FLinearColor(FColor::Green);}
	virtual bool CanUserDeleteNode() const override {return true;}
	virtual bool CanDuplicateNode() const override {return true;}

	void CreateData(UClass* DataClass);
	UEventGraphConditionData* GetData() const
	{
		return Data;
	}
	void SetNewData(UEventGraphConditionData* NewData) {if (Data == nullptr) Data = NewData; Data->OnPropertyChanged.AddUObject(this, &UEventGraphBaseConditionNode::UpdateNode);}

	void UpdateNode();
	

protected:
	UPROPERTY()
	class UEventGraphConditionData* Data;

	
};

