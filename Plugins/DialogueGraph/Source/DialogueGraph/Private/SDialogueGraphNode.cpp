#include "SDialogueGraphNode.h"
#include "DialogueGraphNode.h"
#include "DialogueDefines.h"
#include "SDialogueGraphPin.h"
#include "Components/SlateWrapperTypes.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widgets/SCanvas.h"

void SDialogueGraphNode::Construct(const FArguments& InArgs, UDialogueGraphNode* InNode)
{
	GraphNode = InNode;
	CastNode = InNode;
	NodeData = InNode->GetCastData();
	SetCursor(EMouseCursor::Default);
	
	UpdateGraphNode();
}

void SDialogueGraphNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	LeftNodeBox = SNew(SVerticalBox);
	RightNodeBox = SNew(SVerticalBox);
	SAssignNew(PropertyViewer, SPropertyViewer);
	TArray<FString>HiddenProperty;
	if (CastNode->isAuto())
	{
		HiddenProperty.Add(FString("Duration"));
		HiddenProperty.Add(FString("DefaultAnswer"));
	}
	PropertyViewer->SetObject(NodeData,HiddenProperty);

	const ISlateStyle* StyleSet = FSlateStyleRegistry::FindSlateStyle(DialogueGraphEditorStyle);
	const FSlateBrush* NodeBGBrush = StyleSet->GetBrush("DialogueGraphAsset.NodeBox.Background");
	const FSlateBrush* StartBrush = StyleSet->GetBrush("DialogueGraphAsset.Start");
	

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
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.HAlign(HAlign_Center)
						.MaxHeight(1)
						[
							SNew(SCanvas)
							.Clipping(EWidgetClipping::Inherit)
							+SCanvas::Slot()
							.Position(FVector2D(-15,-30))
							.Size(FVector2D(30,30))
							[
								SNew(SImage)
								.Image(StartBrush)
								.Visibility(CastNode->GetIsStart() ? EVisibility::Visible : EVisibility::Collapsed)
								.ColorAndOpacity(CastNode->GetNodeColor())

							]
						]
						+SVerticalBox::Slot()
						.FillContentHeight(4.0f)
						[
							SNew(SBorder)
							.BorderImage(NodeBGBrush)
							.Padding(FMargin(8.f, 4.f))
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Center)
							.BorderBackgroundColor(CastNode->GetNodeColor())
							[
								SNew(STextBlock)
								.Text(FText::FromString(CastNode->GetNodeTitle()))
								.Font(FAppStyle::GetFontStyle("BoldFont"))
								.Justification(ETextJustify::Center)
								.ColorAndOpacity(FLinearColor::White)
							]
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						[
							PropertyViewer.ToSharedRef()
						]
					]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Fill)
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
						.BorderBackgroundColor(CastNode->GetNodeColor())
						[
							SNew(SBox)
							.Padding(FMargin(0, 0.0))
							[
								RightNodeBox.ToSharedRef()
							]
						]
					]
				]
			]
		]
	];

	CreatePinWidgets();
}

void SDialogueGraphNode::CreatePinWidgets()
{
	for (UEdGraphPin* CurPin : GraphNode->Pins)
	{
		const TSharedRef<SDialogueGraphPin> NewPin = SNew(SDialogueGraphPin, CurPin);
		AddPin(NewPin);
	}
	
}

FVector2D SDialogueGraphNode::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return SGraphNode::ComputeDesiredSize(LayoutScaleMultiplier);
}

void SDialogueGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
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
		if (!CastNode->HasChoice(PinToAdd.Get().GetPinObj())) {CastNode->AddChoice(PinToAdd.Get().GetPinObj()); }
		FAnswerChoiceData& CorrectChoiceData = CastNode->GetChoiceFromPin(PinToAdd.Get().GetPinObj());
		RightNodeBox->AddSlot()
		.AutoHeight()
		.Padding(-10.0f,4.0f)
		[
			CorrectChoiceData.CreateWidget(PinToAdd,CastNode->isAuto()).ToSharedRef()
		];
		OutputPins.Add(PinToAdd);
	}
}
