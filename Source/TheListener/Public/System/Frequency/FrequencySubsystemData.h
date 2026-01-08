#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FrequencySubsystemData.generated.h"

UCLASS()
class THELISTENER_API UFrequencySubsystemData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FVector2D GlobalRange{0,0};

	UPROPERTY(EditDefaultsOnly)
	class UCurveLinearColorAtlas* CurveAtlas{nullptr};

	UPROPERTY(EditDefaultsOnly)
	class UCurveLinearColor* FrequencyColorCurve{nullptr};
	
	UPROPERTY(EditDefaultsOnly)
	class UStationListDataAsset* StartingStationList{nullptr};
};
