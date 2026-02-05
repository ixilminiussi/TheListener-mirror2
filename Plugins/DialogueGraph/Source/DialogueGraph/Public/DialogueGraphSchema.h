#pragma once
#include "CoreMinimal.h"
#include "DialogueGraphLeaveNode.h"
#include "DialogueGraphNode.h"
#include "EdGraph/EdGraph.h"
#include "DialogueGraphSchema.generated.h"

UCLASS()
class UDialogueGraphSchema : public UEdGraphSchema{
	GENERATED_BODY()
	
	public:
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
};

USTRUCT()
struct FNewDialogueNodeAction : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:
	FNewDialogueNodeAction() {}
	FNewDialogueNodeAction(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, UClass* InNodeClass = UDialogueGraphNode::StaticClass())
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), CurrentClass(InNodeClass) {}
		
	UClass* CurrentClass;

	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true);
};

USTRUCT()
struct FNewLeaveDialogueNodeAction : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:
	FNewLeaveDialogueNodeAction() {}
	FNewLeaveDialogueNodeAction(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, UClass* InNodeClass = UDialogueGraphLeaveNode::StaticClass())
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), CurrentClass(InNodeClass) {}
		
	UClass* CurrentClass;

	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true);
};

USTRUCT()
struct FNewAutoDialogueNodeAction : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:
	FNewAutoDialogueNodeAction() {}
	FNewAutoDialogueNodeAction(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, UClass* InNodeClass = UDialogueGraphNode::StaticClass())
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), CurrentClass(InNodeClass) {}
		
	UClass* CurrentClass;

	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true);
};
