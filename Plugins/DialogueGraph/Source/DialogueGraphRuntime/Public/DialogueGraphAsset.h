// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DialogueGraphAsset.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPreSave);
/**
 * 
 */
UCLASS()
class DIALOGUEGRAPHRUNTIME_API UDialogueGraphAsset : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<class UDialogueLineData*> LineDatas;
	UPROPERTY()
	TArray<class UEventGraphConditionData*> ConditionDatas;
	UPROPERTY()
	class UDialogueLineData* Start;
	UPROPERTY()
	class UDialogueGraphData* Graph;
	
	#if  UE_EDITOR
	FOnPreSave OnPreSave;
    
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;
    #endif
};
