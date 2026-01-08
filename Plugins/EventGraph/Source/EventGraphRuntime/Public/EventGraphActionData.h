// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EventGraphActionData.generated.h"

/**
 * 
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced)
class EVENTGRAPHRUNTIME_API UEventGraphActionData : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void LaunchEvent(const UObject* InWorldContextObject) PURE_VIRTUAL(
		UEventGraphActionData::LaunchEvent, return void(););

	UPROPERTY(Category = "Action", EditAnywhere, meta = (ClampMin = "0"))
	float ActionDelay = 0.0f;

	virtual struct FActionKey GetKey() const;
	static struct FActionKey GenerateKey();
};

UCLASS(meta=(DisplayName="Debug Action"))
class EVENTGRAPHRUNTIME_API UDebugEventGraphActionData : public UEventGraphActionData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString DebugString;
	
	virtual void LaunchEvent(const UObject* InWorldContextObject) override;

	virtual struct FActionKey GetKey() const;
	static struct FActionKey GenerateKey(FString DebugString);
};
