#pragma once

#include "CoreMinimal.h"
#include "GPE/Radio/Station.h"

/*
UCLASS()
class THELISTENER_API APrimaryStation : public AStation
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	

public:
	virtual void SetupStation(class UStationAsset* StationDataAsset, class ARadio* RadioActor) override;
	float GetCurrentInterest(const float CurrentFrequency) const;

	void UpdateText() const;
	void LaunchStation();
	virtual void OnAkEventCallback(EAkCallbackType CallbackType, UAkCallbackInfo* CallbackInfo) override;
	UFUNCTION(BlueprintCallable)
	void OnStationFinished();

protected:
	UPROPERTY()
	class ADecoder* Decoder{nullptr};

public:
	void SetDecoder(class ADecoder* DecoderActor);
	bool WasTrue = false;

private:
	bool IsGameClosing = false;
};
*/
