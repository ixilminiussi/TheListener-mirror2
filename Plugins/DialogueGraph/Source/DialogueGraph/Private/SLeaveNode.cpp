#include "SLeaveNode.h"

#include "Styling/SlateStyleRegistry.h"
#include "DialogueDefines.h"
#include "DialogueGraphLeaveNode.h"
#include "SDialogueGraphPin.h"

void SLeaveNode::Construct(const FArguments& InArgs, class UDialogueGraphLeaveNode* InNode)
{
	SetCursor(EMouseCursor::Default);
	GraphNode = InNode;
	CastNode = InNode;
	UpdateGraphNode();
}

void SLeaveNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	LeftNodeBox = SNew(SVerticalBox);
	RightNodeBox = SNew(SVerticalBox);
	const ISlateStyle* StyleSet = FSlateStyleRegistry::FindSlateStyle(DialogueGraphEditorStyle);
	const FSlateBrush* NodeBGBrush = StyleSet->GetBrush("DialogueGraphAsset.NodeBox.Background");
	const FSlateBrush* LeaveBrush = StyleSet->GetBrush("DialogueGraphAsset.Leave");

	//EventGraphNodePropertyLoader::LoadProperty(NodeData,PropertyVerticalBox);
	
	GetOrAddSlot(ENodeZone::Center)
	[
		SNew(SBorder)
		.BorderImage(NodeBGBrush)
		.BorderBackgroundColor(CastNode->GetNodeColor())
		.Clipping(EWidgetClipping::OnDemand)
		.Padding(2.0f,2.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(SBox)
				.Padding(0,0)
				[
					LeftNodeBox.ToSharedRef()
				]
			]
			+ SHorizontalBox::Slot()
			.MinWidth(100)
			.MaxWidth(500)
			.AutoWidth()
			[
				SNew(SBorder)
				.BorderImage(NodeBGBrush)
				.HAlign(HAlign_Fill)
				.BorderBackgroundColor(FLinearColor(0.02, 0.02, 0.025))
				[
					SNew(SBorder)
					.BorderImage(NodeBGBrush)
					.Padding(FMargin(8.f, 4.f))
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					.BorderBackgroundColor(CastNode->GetNodeColor())
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.HAlign(HAlign_Center)
						.Padding(4.0f)
						.FillContentHeight(1.0)
						[
							SNew(SImage)
							.Image(LeaveBrush)
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						[
								SNew(STextBlock)
								.Text(FText::FromString(CastNode->GetNodeTitle()))
								.Font(FAppStyle::GetFontStyle("BoldFont"))
								.Justification(ETextJustify::Center)
								.ColorAndOpacity(FLinearColor::White)
						]
					]
				]
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoWidth()
			[
				SNew(SBox)
				.Padding(0,0)
				[
					RightNodeBox.ToSharedRef()
				]
			]
		]
	];

	CreatePinWidgets();
}

void SLeaveNode::CreatePinWidgets()
{
	for (UEdGraphPin* CurPin : GraphNode->Pins)
	{
		const TSharedRef<SDialogueGraphPin> NewPin = SNew(SDialogueGraphPin, CurPin);
		AddPin(NewPin);
	}
}

FVector2D SLeaveNode::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return SGraphNode::ComputeDesiredSize(LayoutScaleMultiplier);
}

void SLeaveNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	if (PinToAdd->GetDirection() == EGPD_Input)
	{
		LeftNodeBox->AddSlot()
		.AutoHeight()
		.Padding(-10.0f,4.0f)
		[
			PinToAdd
		];
		InputPins.Add(PinToAdd);
	}
	else
	{
		RightNodeBox->AddSlot()
		.AutoHeight()
		.Padding(-10.0f,4.0f)
		[
			PinToAdd
		];
		OutputPins.Add(PinToAdd);
	}
}
