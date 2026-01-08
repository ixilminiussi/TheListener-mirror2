// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StationListDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API UStationListDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	const TArray<class UStationAsset*>& GetStationsData() const;

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<class UStationAsset*> CurrentStationList{};
};
