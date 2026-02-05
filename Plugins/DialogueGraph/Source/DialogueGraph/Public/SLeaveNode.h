#pragma once

#include "SDialogueChoice.h"
#include "SGraphNode.h"
#include "SPropertyViewer.h"

class SLeaveNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SLeaveNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, class UDialogueGraphLeaveNode* InNode);

protected:
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;

	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

	UPROPERTY()
	class UDialogueGraphLeaveNode* CastNode;
};
