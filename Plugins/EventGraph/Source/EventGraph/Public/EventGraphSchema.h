#pragma once
#include "CoreMinimal.h"
#include "EventGraphBaseConditionNode.h"
#include "EdGraph/EdGraph.h"
#include "EventGraphSchema.generated.h"

UCLASS()
class UEventGraphSchema : public UEdGraphSchema{
	GENERATED_BODY()
	
	public:
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
};

USTRUCT()
struct FNewConditionNodeAction : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:
	FNewConditionNodeAction() {}
	FNewConditionNodeAction(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, UClass* InNodeClass = UEventGraphBaseConditionNode::StaticClass())
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), CurrentClass(InNodeClass) {}
		
	UClass* CurrentClass;

	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true);
};

USTRUCT()
struct FNewEventNodeAction : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:
	FNewEventNodeAction() {}
	FNewEventNodeAction(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping) {}
	
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true);
};

USTRUCT()
struct FNewLogicConditionNodeAction : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:
	FNewLogicConditionNodeAction() {}
	FNewLogicConditionNodeAction(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping,  UClass* InNodeClass = UEventGraphLogicConditionData::StaticClass())
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), CurrentClass(InNodeClass) {}

	UClass* CurrentClass;
	
	virtual UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true);
};

