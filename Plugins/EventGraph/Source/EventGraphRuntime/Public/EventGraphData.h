// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventGraphConditionData.h"
#include "UObject/Object.h"
#include "EventGraphData.generated.h"

/**
 * 
 */
UCLASS()
class EVENTGRAPHRUNTIME_API UEventGraphPinRuntime : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName PinName;

	UPROPERTY()
	FGuid PinGuid;

	UPROPERTY()
	TArray<UEventGraphPinRuntime*> Connection;
};

UCLASS()
class EVENTGRAPHRUNTIME_API UEventGraphNodeRuntime : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UEventGraphPinRuntime* ConditionOutputPin;

	UPROPERTY()
	TArray<UEventGraphPinRuntime*> ConditionInputPins;

	UPROPERTY()
	FVector2f Position;

	UPROPERTY()
	UEventGraphConditionData* Data;
};

UCLASS()
class EVENTGRAPHRUNTIME_API UEventGraphData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UEventGraphNodeRuntime*> Nodes;

	UPROPERTY()
	TArray<UEventGraphEventConditionData*> Events;
	
};
