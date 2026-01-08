// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventGraphBaseConditionNode.h"
#include "EventGraphEventConditionNode.generated.h"

/**
 * 
 */
UCLASS()
class EVENTGRAPH_API UEventGraphEventConditionNode : public UEventGraphBaseConditionNode
{
	GENERATED_BODY()

	virtual void AllocateDefaultPins() override;

protected:
	UEdGraphPin* InputCondition;
};
