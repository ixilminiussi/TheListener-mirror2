#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "FrequencySubsystem.generated.h"

UCLASS()
class THELISTENER_API UFrequencySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void OnActorsInitialized(const FActorsInitializedParams& Params);
	void InitEvents();
	virtual void Deinitialize() override;

	void RegisterReceiver(class UMaterialInstanceDynamic* Material);
	void RegisterReceiver(class ARadio* Radio);

	void RefreshCurveAtlas() const;
	void AddStation(class UStationAsset* StationData);
	void DestroyStation(const class UStationAsset* StationData) const;
	void ForgetStation(class AStation* Station);

	const TArray<class UStationAsset*>& GetStationDataAssets() const
	{
		return StationDataAssets;
	}

protected:
	UPROPERTY()
	TArray<class UMaterialInstanceDynamic*> ReceiverRegister;

	FVector2D GlobalRange;

	static void ClearAndZeroCurve(FRichCurve* Curve);
	void CurveFlush(class UCurveLinearColorAtlas* Atlas) const;
	void AddFrequencyToCurve(FRichCurve* Curve, const class UStationAsset* StationData) const;
	void RemoveFrequencyFomCurve(FRichCurve* Curve, const class UStationAsset* StationData) const;

	UPROPERTY()
	class UCurveLinearColorAtlas* CurveAtlas;
	UPROPERTY()
	class UCurveLinearColor* FrequencyColorCurve;

	UPROPERTY()
	class UStationListDataAsset* StartingStationList;
	// Data Asset that contains all stations data assets
	UPROPERTY()
	TArray<class UStationAsset*> StationDataAssets;

	UPROPERTY()
	TArray<TObjectPtr<class ARadio>> Radios;
};
