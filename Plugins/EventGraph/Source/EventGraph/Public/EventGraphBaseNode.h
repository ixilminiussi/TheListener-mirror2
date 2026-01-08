#pragma once

#include "EdGraph/EdGraphNode.h"
#include "EventGraphBaseNode.generated.h"

UCLASS()
class EVENTGRAPH_API UEventGraphBaseNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	void Setup(class UEdGraph* ParentGraph, class UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode);
	virtual void AllocateDefaultPins() override;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
	virtual void PostEditUndo() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	UFUNCTION()
	FEdGraphPinReference AddInputPins();
	UFUNCTION()
	FEdGraphPinReference AddInputPinsID(FGuid NewID);
	UFUNCTION()
	void RemoveInputPins();
	UFUNCTION()
	void RemoveAllInputPins();
	UFUNCTION()
	void ChangeOutputPinsID(FGuid NewID);

	UFUNCTION()
	void ReloadPins();

	UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, FName Name);

	TArray<FEdGraphPinReference> GetInputPins()
	{
		return InputPins;
	}

	FEdGraphPinReference GetOutputPins()
	{
		return ConditionOutputPin;
	}
	
protected:
	UPROPERTY()
	FEdGraphPinReference ConditionOutputPin;
	UPROPERTY()
	TArray<FEdGraphPinReference> InputPins = TArray<FEdGraphPinReference>();

	
};