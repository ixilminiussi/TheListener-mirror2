// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "DialogueGraphBaseNode.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUEGRAPH_API UDialogueGraphBaseNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	void Setup(class UEdGraph* ParentGraph, class UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode, bool bIsCreated = true);

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	
	virtual void PostEditUndo() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	virtual FString GetNodeTitle() const {return FString("Dialogue Line");}
	virtual FLinearColor GetNodeColor() const ;

	UEdGraphPin* CreateCustomPin(EEdGraphPinDirection Direction, FGuid PinID = FGuid::NewGuid(), FName PinCategory = TEXT("DialoguePin"));

	virtual void PostPlacedNewNode() override;

	virtual void CreateData(){}
	virtual UObject* GetData() {return nullptr;};
	virtual UClass* GetDataClass() {return UObject::StaticClass();};
	virtual void SetData(UObject* Object) {};
	
	virtual void SaveDataAdditional(class UDialogueGraphNodeRuntime*);
	
	TArray<FEdGraphPinReference> GetInputPins(){return InputPins;};
	TArray<FEdGraphPinReference> GetOutputPins(){return OutputPins;};

protected:

	UPROPERTY()
	TArray<FEdGraphPinReference> InputPins;
	UPROPERTY()
	TArray<FEdGraphPinReference> OutputPins;
	
	
	
	
	
};
