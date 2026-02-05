// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DialogueGraphData.generated.h"

UCLASS()
class DIALOGUEGRAPHRUNTIME_API UDialogueGraphPinRuntime : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName PinCategory;

	UPROPERTY()
	FName PinName;

	UPROPERTY()
	FGuid PinGuid;

	UPROPERTY()
	TArray<UDialogueGraphPinRuntime*> Connection;
};

UCLASS()
class DIALOGUEGRAPHRUNTIME_API UDialogueGraphNodeRuntime : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UDialogueGraphPinRuntime*> OutputPins;

	UPROPERTY()
	TArray<UDialogueGraphPinRuntime*> InputPins;

	UPROPERTY()
	UClass* NodeClass;

	UPROPERTY()
	FVector2f Position;

	UPROPERTY()
	class UObject* NodeData;
};


UCLASS()
class DIALOGUEGRAPHRUNTIME_API UDialogueGraphData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UDialogueGraphNodeRuntime*> Nodes;
};
