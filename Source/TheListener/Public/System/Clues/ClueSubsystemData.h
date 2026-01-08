// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ClueSubsystemData.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API UClueSubsystemData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<class UClueAsset*> CluesList;
};
