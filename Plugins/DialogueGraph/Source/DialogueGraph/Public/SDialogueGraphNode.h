#pragma once

#include "SDialogueChoice.h"
#include "SGraphNode.h"
#include "SPropertyViewer.h"

class SDialogueGraphNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SDialogueGraphNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, class UDialogueGraphNode* InNode);
	

protected:
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;

	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

	UPROPERTY()
	class UDialogueGraphNode* CastNode = nullptr;
	UPROPERTY()
	class UDialogueLineData* NodeData = nullptr;

	UPROPERTY()
	TSharedPtr<SPropertyViewer> PropertyViewer;

	
};
