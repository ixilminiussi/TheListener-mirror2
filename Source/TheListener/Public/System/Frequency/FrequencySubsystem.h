#pragma once

#include "CoreMinimal.h"
#include "FrequencySubsystemData.h"
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
	void RegisterReceiver(class ASDR* SDR);

	void RefreshCurveAtlas() const;
	void AddStation(class UStationAsset* StationData);
	void DestroyStation(const class UStationAsset* StationData) const;
	void ForgetStation(class AStation* Station);

	bool IsFrequencyAllowed(float Value) const;

	bool NextBand();
	bool LastBand();
	void SetAllowedBands(TArray<int> InAllowedBands);
	void AllowBand(int InBand);
	void SetBandFromFrequency(float Frequency);

	const TArray<class UStationAsset*>& GetStationDataAssets() const
	{
		return StationDataAssets;
	}

protected:
	void SetBand(uint32);
	
	UPROPERTY()
	TArray<class UMaterialInstanceDynamic*> ReceiverRegister;

	UPROPERTY()
	TArray<FVector2D> ReceptionBands;
	uint32 Band;
	TArray<int> AllowedBands;

	UPROPERTY(EditDefaultsOnly)
	class UCurveLinearColorAtlas* CurveAtlas{nullptr};

	UPROPERTY(EditDefaultsOnly)
	class UCurveLinearColor* FrequencyColorCurve{nullptr};

	static void ClearAndZeroCurve(FRichCurve* Curve);
	void CurveFlush(class UCurveLinearColorAtlas* Atlas) const;
	void AddFrequencyToCurve(FRichCurve* Curve, const class UStationAsset* StationData) const;
	void RemoveFrequencyFomCurve(FRichCurve* Curve, const class UStationAsset* StationData) const;

	UPROPERTY()
	class UStationListDataAsset* StartingStationList;
	// Data Asset that contains all stations data assets
	UPROPERTY()
	TArray<class UStationAsset*> StationDataAssets;

	TMap<TObjectPtr<class ARadio>, TArray<float>> Radios;
	TArray<TObjectPtr<class ASDR>> SDRs;
};
