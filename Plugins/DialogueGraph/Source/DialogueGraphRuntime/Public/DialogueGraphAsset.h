// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DialogueGraphAsset.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUEGRAPHRUNTIME_API UDialogueGraphAsset : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category="Dialogue")
	FString Name;

	TArray<class UDialogueLineData*> Lines;
};
