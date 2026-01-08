// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventGraphBaseConditionNode.h"
#include "EventGraphLogicNode.generated.h"

/**
 * 
 */
UCLASS()
class EVENTGRAPH_API UEventGraphLogicNode : public UEventGraphBaseConditionNode
{
	GENERATED_BODY()
public:
	virtual void AllocateDefaultPins() override;
	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;
};
