#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "ListenerWorldSettings.generated.h"

UCLASS(Blueprintable)
class THELISTENER_API AListenerWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = "Listener|Investigation", EditDefaultsOnly)
	TSoftObjectPtr<class UClueSubsystemData> ClueSubsystemData{nullptr};

	UPROPERTY(Category = "Listener|Frequency", EditDefaultsOnly)
	TSoftObjectPtr<class UFrequencySubsystemData> FrequencySubsystemData{nullptr};

	UPROPERTY(Category = "Listener", EditDefaultsOnly)
	TArray<TSoftObjectPtr<UWorld>> AllowedLevels{};
};