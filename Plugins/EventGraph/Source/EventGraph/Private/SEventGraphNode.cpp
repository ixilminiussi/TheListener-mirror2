#include "SEventGraphNode.h"

#include "EventGraphBaseConditionNode.h"
#include "EventGraphConditionData.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "SGraphPin.h"
#include "SEventGraphConditionPin.h"
#include "EventGraphDefines.h"
#include "EventGraphNodeProperty.h"
#include "Styling/SlateStyleRegistry.h"

void SEventGraphNode::Construct(const FArguments& InArgs, UEventGraphBaseConditionNode* InNode)
{
	GraphNode = InNode;
	NodeData = InNode->GetData();
	SetCursor(EMouseCursor::CardinalCross);
	
	UpdateGraphNode();
}

void SEventGraphNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	LeftNodeBox = SNew(SVerticalBox);
	RightNodeBox = SNew(SVerticalBox);
	SAssignNew(PropertyVerticalBox, SVerticalBox);

	const ISlateStyle* StyleSet = FSlateStyleRegistry::FindSlateStyle(EventGraphEditorStyle);
	const FSlateBrush* NodeBGBrush = StyleSet->GetBrush("EventGraphAsset.NodeBox.Background");

	EventGraphNodePropertyLoader::LoadProperty(NodeData,PropertyVerticalBox);
	
	EVisibility ContentVisibility = NodeData->GetType() == EEventGraphConditionType::Logic ? EVisibility::Collapsed : EVisibility::Visible;
	float NodeMinWidth = NodeData->GetType() == EEventGraphConditionType::Logic ? 0 : 100;
	GetOrAddSlot(ENodeZone::Center)
	[
		SNew(SBorder)
		.BorderImage(NodeBGBrush)
		.BorderBackgroundColor(NodeData->GetNodeColor())
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
			.MinWidth(NodeMinWidth)
			.MaxWidth(500)
			.AutoWidth()
			[
					SNew(SBorder)
					.BorderImage(NodeBGBrush)
					.HAlign(HAlign_Fill)
					.BorderBackgroundColor(FLinearColor(0.02, 0.02, 0.025))
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.FillHeight(1.0f)
						[
							SNew(SBorder)
							.BorderImage(NodeBGBrush)
							.Padding(FMargin(8.f, 4.f))
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Center)
							.BorderBackgroundColor(NodeData->GetTrueNodeColor())
							[
								SNew(STextBlock)
								.Text(NodeData->GetNodeTitle())
								.Font(FAppStyle::GetFontStyle("BoldFont"))
								.Justification(ETextJustify::Center)
								.ColorAndOpacity(FLinearColor::White)
							]
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						[
							SNew(SBox)
							.Visibility(ContentVisibility)
							[
								PropertyVerticalBox.ToSharedRef()
							]
						]
					]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.Padding(FMargin(0, 0.0))
				[
				RightNodeBox.ToSharedRef()
				]
			]
		]
	];

	CreatePinWidgets();
}

void SEventGraphNode::CreatePinWidgets()
{
	for (UEdGraphPin* CurPin : GraphNode->Pins)
	{
		const TSharedRef<SEventGraphConditionPin> NewPin = SNew(SEventGraphConditionPin, CurPin);
		AddPin(NewPin);
	}
}

FVector2D SEventGraphNode::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return SGraphNode::ComputeDesiredSize(LayoutScaleMultiplier);
}

void SEventGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	//SGraphNode::AddPin(PinToAdd);
	//return;
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

void SEventGraphNode::CreateProperty(FProperty* Property, UObject* ObjectData)
{
	/*
	if (CastField<FFloatProperty>(Property))
	{
		PropertyVerticalBox->AddSlot()
		.Padding(FMargin(2, 0.0f))
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.MinWidth(100)
			.MaxWidth(100)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Property->GetName()))
			]
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.FillWidth(1.0f)
			[
				SNew(SNumericEntryBox<float>)
				.Value_Lambda([Property, ObjectData]()
				{
					float Value = 0;
					FFloatProperty* CastProperty = CastField<FFloatProperty>(Property);
					if (CastProperty)
					{
						Value = CastProperty->GetPropertyValue_InContainer(ObjectData);
					}
					return Value;
				})
				.OnValueChanged_Lambda([Property, ObjectData](float NewValue)
				{
					FFloatProperty* CastProperty = CastField<FFloatProperty>(Property);
					if (CastProperty)
					{
						CastProperty->SetPropertyValue_InContainer(ObjectData,NewValue);
					}
				})
			]
		];
	}*/
}
