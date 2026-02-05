#include "DialogueGraphNode.h"
#include "SDialogueChoice.h"
#include "DialogueDefines.h"
#include "DialogueEdGraph.h"
#include "DialogueGraphData.h"
#include "DialogueGraphEditorApp.h"

TSharedPtr<SDialogueChoice> FAnswerChoiceData::CreateWidget(TSharedRef<SGraphPin> GraphPin, bool isCompact)
{
	FString LabelTxt = FString::FromInt(AnswerIndex);
	LabelTxt.Append(" : ");
	return Widget = SNew(SDialogueChoice)
		.GraphPin(GraphPin)
		.OnTextCommited_Raw(this, &FAnswerChoiceData::UpdateText)
		.Compact(isCompact)
		.Text(FText::FromString(AnswerText))
		.LabelText(FText::FromString(LabelTxt));
}

FAnswerChoiceData::FAnswerChoiceData()
{
}

FAnswerChoiceData::FAnswerChoiceData(FEdGraphPinReference NewPinReference)
{
	PinReference = NewPinReference;
	AnswerIndex = -1;
}



void FAnswerChoiceData::UpdateText(const FText& NewText, ETextCommit::Type CommitType)
{
	AnswerText = NewText.ToString();
}

void UDialogueGraphNode::AllocateDefaultPins()
{
	CreateCustomPin(EGPD_Input, FGuid::NewGuid(), TEXT("DialoguePin"));
}

void UDialogueGraphNode::GetNodeContextMenuActions(class UToolMenu* Menu,
                                                       class UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);
	FToolMenuSection& AnsSection = Menu->AddSection(TEXT("Answer"),FText::FromString(TEXT("Dialogue Node Actions")));
	FToolMenuSection& DialogueSection = Menu->AddSection(TEXT("Global"),FText::FromString(TEXT("Dialogue Node Actions")));

	UDialogueGraphNode* Node = (UDialogueGraphNode*)this;
	AnsSection.AddMenuEntry(
		TEXT("AddAnswer"),
		FText::FromString(TEXT("Add Answer")),
		FText::FromString(TEXT("Create A New Answer")),
		FSlateIcon(DialogueGraphEditorStyle, TEXT("DialogueGraphAsset.Add")),
		FUIAction(FExecuteAction::CreateLambda(
		[Node](){
					Node->AddChoice(nullptr);
				}
			))
		);

	AnsSection.AddMenuEntry(
		TEXT("RemoveAnswer"),
		FText::FromString(TEXT("Remove an Answer")),
		FText::FromString(TEXT("Remove one Answer (prioritizing unconnected answer and last answer)")),
		FSlateIcon(DialogueGraphEditorStyle, TEXT("DialogueGraphAsset.Remove")),
		FUIAction(FExecuteAction::CreateLambda(
		[Node](){
					Node->RemoveChoice();
				}
			))
		);
	FString AutoTile = isAuto() ? TEXT("Remove Auto") : TEXT("Toggle Auto");
	DialogueSection.AddMenuEntry(
		TEXT("ToggleAuto"),
		FText::FromString(TEXT("Set To Auto")),
		FText::FromString(TEXT("Toggle Auto State of this node")),
		FSlateIcon(DialogueGraphEditorStyle, TEXT("DialogueGraphAsset.Auto")),
		FUIAction(FExecuteAction::CreateLambda(
		[Node](){
					Node->ToggleAuto();
				}
			))
		);

	DialogueSection.AddMenuEntry(
		TEXT("SetStart"),
		FText::FromString(TEXT("Set Start")),
		FText::FromString(TEXT("Select a node to start the dialogue")),
		FSlateIcon(DialogueGraphEditorStyle, TEXT("DialogueGraphAsset.Start")),
		FUIAction(FExecuteAction::CreateLambda(
		[Node](){
					UDialogueEdGraph* Graph = CastChecked<UDialogueEdGraph>(Node->GetGraph());
					Graph->SetStartNode(Node);
				}
			))
		);
}


FLinearColor UDialogueGraphNode::GetNodeColor() const
{
	return isStartNode ? FLinearColor(FColor(230,154,20,255)) : FLinearColor::Green;
}

void UDialogueGraphNode::AddChoice(UEdGraphPin* Pin = nullptr)
{
	if (Pin == nullptr) {Pin = CreateCustomPin(EGPD_Output);}
	FAnswerChoiceData NewChoice = FAnswerChoiceData(Pin);
	Choices.Add(NewChoice);
	RefreshChoiceIndex();
}

void UDialogueGraphNode::RemoveChoice()
{
	if (Choices.Num() < 1) {return;}
	int ChoiceToDelete = -1;
	for (int i = Choices.Num()-1; i >= 0; i--)
	{
		FAnswerChoiceData Choice = Choices[i];
		if (Choice.PinReference.Get()->LinkedTo.Num() == 0 )
		{
			ChoiceToDelete = i;
			break;
		}
	}
	if (ChoiceToDelete == -1){ChoiceToDelete = Choices.Num() - 1;}
	UEdGraphPin* Pin = Choices[ChoiceToDelete].PinReference.Get();
	Choices.RemoveAt(ChoiceToDelete);
	Pin->BreakAllPinLinks();
	OutputPins.Remove(Pin);
	RemovePin(Pin);
	Modify();
	RefreshChoiceIndex();
}

void UDialogueGraphNode::RefreshChoiceIndex()
{
	for (int i = 0; i < Choices.Num(); ++i)
	{
		Choices[i].AnswerIndex = i;
	}
	GetGraph()->NotifyNodeChanged(this);
}

void UDialogueGraphNode::PostEditUndo()
{
	SetIsStartNode(Cast<UDialogueEdGraph>(GetGraph())->GetStartNode() == this);
	Super::PostEditUndo();
}

FAnswerChoiceData& UDialogueGraphNode::GetChoiceFromPin(const FEdGraphPinReference& PinRef )
{
	
	for (FAnswerChoiceData& Choice : Choices)
	{
		if (Choice.PinReference == PinRef) {return Choice;}
	}
	return Choices[0];
}

bool UDialogueGraphNode::HasChoice(const FEdGraphPinReference& PinRef)
{
	for (FAnswerChoiceData& Choice : Choices)
	{
		if (Choice.PinReference == PinRef) {return true;}
	}
	return false;
}

void UDialogueGraphNode::ToggleAuto() const
{
	Data->bIsAuto = !Data->bIsAuto;
	GetGraph()->NotifyNodeChanged(this);
}

void UDialogueGraphNode::CreateData()
{
	Data = NewObject<UDialogueLineData>();
}

void UDialogueGraphNode::SetData(UObject* Object)
{
	Data = Cast<UDialogueLineData>(Object);
	if (Data->IsStart)
	{
		UDialogueEdGraph* Graph = CastChecked<UDialogueEdGraph>(GetGraph());
		Graph->SetStartNode(this);
	}
	Choices.Empty();
	for (FEdGraphPinReference& PinRef : OutputPins)
	{
		FAnswerChoiceData Choice = FAnswerChoiceData(PinRef);
		if (Data->Next.Num() > Choices.Num())
		{
			Choice.AnswerText = Data->Next[Choices.Num()]->Text;
		}
		Choices.Add(Choice);
	}
	RefreshChoiceIndex();
}

void UDialogueGraphNode::SetIsStartNode(bool IsItStart)
{
	isStartNode = IsItStart;
}

bool UDialogueGraphNode::GetIsStart()
{
	return isStartNode;
}

int UDialogueGraphNode::GetInputsNumber()
{
	//TODO REplace Array by two variables
	return InputPins[0].Get()->LinkedTo.Num();
}

void UDialogueGraphNode::SaveDataAdditional(UDialogueGraphNodeRuntime* RuntimeNode)
{
	UDialogueLineData* CastData = Cast<UDialogueLineData>(RuntimeNode->NodeData);
	CastData->Next.Empty();
	CastData->IsStart = isStartNode;
	for (FAnswerChoiceData& Choice : Choices)
	{
		UAnswer* NewAnswer = NewObject<UAnswer>(RuntimeNode->NodeData);
		if (!NewAnswer)
		{
			continue;
		}
		NewAnswer->Text = Choice.AnswerText;
		UEdGraphPin* NewPin = Choice.PinReference.Get();
		if (ensure(NewPin))
		{
			NewAnswer->PinGuid = NewPin->LinkedTo.Num() > 0 ? NewPin->LinkedTo[0]->PinId : FGuid();
		}
		
		CastData->Next.Add(NewAnswer);
	}
}




