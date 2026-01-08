#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EventGraphPluginSettings.generated.h"

UCLASS(config=Game, defaultconfig, meta=(DisplayName="Event Graph"))
class EVENTGRAPHRUNTIME_API UEventGraphPluginSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
   UPROPERTY(Category = "Levels", EditAnywhere, config)
   	TArray<TSoftObjectPtr<UWorld>> AllowedLevels{};
    UPROPERTY(Category = "Events",EditAnywhere, config)
    TSoftObjectPtr<class UEventGraphAsset> StartEventGraph;
public:
    // Optional override to define where in Project Settings it appears
    virtual FName GetCategoryName() const override { return FName("Event Graph"); }

    virtual FName GetSectionName() const override { return FName("Event Graph"); }
};