#pragma once

#include "SGraphNode.h"

class SEventGraphNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SEventGraphNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, class UEventGraphBaseConditionNode* InNode);

protected:
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;

	void UpdateValue();

	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

	void CreateProperty(FProperty* Property, UObject* ObjectData);

	class UEventGraphConditionData* NodeData = nullptr;
	
	TSharedPtr<SVerticalBox> PropertyVerticalBox;
	
};


