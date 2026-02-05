#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FrequencySubsystemData.generated.h"

UCLASS()
class THELISTENER_API UFrequencySubsystemData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	FVector2D LfRange{30,300};
	UPROPERTY(VisibleAnywhere)
	FVector2D MfRange{300,3000};
	UPROPERTY(VisibleAnywhere)
	FVector2D HfRange{3000,30000};

	UPROPERTY(EditDefaultsOnly)
	class UCurveLinearColorAtlas* CurveAtlas{nullptr};

	UPROPERTY(EditDefaultsOnly)
	class UCurveLinearColor* FrequencyColorCurve{nullptr};
	
	UPROPERTY(EditDefaultsOnly)
	class UStationListDataAsset* StartingStationList{nullptr};
};
