#pragma once

#include "DialogueGraphBaseNode.h"
#include "EdGraph/EdGraphNode.h"
#include "DialogueGraphRuntime/Public/DialogueLineDataFuture.h"
#include "SDialogueChoice.h"

#include "DialogueGraphNode.generated.h"

USTRUCT()
struct FAnswerChoiceData
{
	GENERATED_BODY()
	FEdGraphPinReference PinReference;
	TSharedPtr<SDialogueChoice> Widget;
	FString AnswerText = "Answer ";
	int AnswerIndex = -2;

	TSharedPtr<SDialogueChoice> CreateWidget(TSharedRef<SGraphPin> GraphPin, bool isCompact);

	FAnswerChoiceData();
	FAnswerChoiceData(FEdGraphPinReference PinReference);

private:
	void UpdateText(const FText& NewText, ETextCommit::Type CommitType);
};

UCLASS()
class UDialogueGraphNode : public UDialogueGraphBaseNode
{
	GENERATED_BODY()
public:
	
	virtual void AllocateDefaultPins() override;
	void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const;
	FLinearColor GetNodeColor() const;

	void AddChoice(UEdGraphPin* Pin);
	void RemoveChoice();
	void RefreshChoiceIndex();

	virtual void PostEditUndo() override;

	FAnswerChoiceData& GetChoiceFromPin(const FEdGraphPinReference& PinRef);
	bool HasChoice(const FEdGraphPinReference& PinRef);

	bool isAuto() const {return Data->IsAutoChoice();}
	void ToggleAuto() const;

	virtual void CreateData() override;
	UDialogueLineData* GetCastData() {return Data;};
	UObject* GetData() override {return Data;};
	UClass* GetDataClass() override {return UDialogueLineData::StaticClass();};
	void SetData(UObject* Object) override ;

	void SetIsStartNode(bool StartNode);
	bool GetIsStart();

	int GetInputsNumber();
	
	virtual void SaveDataAdditional(class UDialogueGraphNodeRuntime*) override;

private:

	UPROPERTY()
	TArray<FAnswerChoiceData> Choices;

	UPROPERTY()
	bool isStartNode = false;
	
	UPROPERTY()
	TObjectPtr<UDialogueLineData> Data;
};
