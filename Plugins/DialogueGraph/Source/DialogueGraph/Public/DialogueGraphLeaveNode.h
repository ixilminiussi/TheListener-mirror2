// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DialogueGraphBaseNode.h"
#include "DialogueLineDataFuture.h"
#include "DialogueGraphLeaveNode.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUEGRAPH_API UDialogueGraphLeaveNode : public UDialogueGraphBaseNode
{
	GENERATED_BODY()
public:
	virtual void AllocateDefaultPins() override;
	void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const;
	virtual FLinearColor GetNodeColor() const override;
	virtual FString GetNodeTitle() const override;

	void CreateData() override;
	UDialogueLineData* GetCastData() {return Data;};
	UObject* GetData() override {return Data;};
	UClass* GetDataClass() override {return UDialogueLineData::StaticClass();};
	void SetData(UObject* Object) override ;

private:
	UPROPERTY()
	TObjectPtr<UDialogueLineData> Data;
};
