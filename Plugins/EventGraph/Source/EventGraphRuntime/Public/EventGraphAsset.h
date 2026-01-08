// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventGraphData.h"
#include "UObject/NoExportTypes.h"
#include "EventGraphAsset.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPreSave);

/**
 * 
 */
UCLASS(BlueprintType)
class EVENTGRAPHRUNTIME_API UEventGraphAsset : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FString Name;

	UPROPERTY()
	UEventGraphData* GraphData = nullptr;

	FOnPreSave OnPreSave;
#if  UE_EDITOR
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif
};
