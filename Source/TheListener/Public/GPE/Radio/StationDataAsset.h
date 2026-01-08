// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Station.h"
#include "Engine/DataAsset.h"
#include "StationDataAsset.generated.h"

UCLASS()
class UQNRTInfoDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	class UConstantQNRT *GetConstantQNRT(class UAkAudioEvent *) const;

protected:
	UPROPERTY(EditAnywhere)
	TMap<TObjectPtr<class UAkAudioEvent>, TObjectPtr<class UConstantQNRT>> QNRTMap;
};

UCLASS()
class THELISTENER_API UStationAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AStation> StationClass = AStation::StaticClass();

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	FString StationName{};

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<class UAkAudioEvent> StartAudioEvent;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<class UAkAudioEvent> StopAudioEvent;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<class UAkRtpc> Rtpc;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Audio",
		meta = (ClampMin = "0.01", ClampMax = "20000.0", DisplayPrecision = "2"))
	float Frequency = .0f;

	UPROPERTY(EditDefaultsOnly, Category = "Audio",
		meta = (ClampMin = "0.01", ClampMax = "20000.0", DisplayPrecision = "2"))
	float ReceptionBand = 50.f;

	TStaticArray<struct FKeyHandle, 3> CurveKeys{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<class UDialogueTreeData> DialogueTreeData = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Decoder")
	bool bIsEncoded = false;

	UPROPERTY(EditDefaultsOnly, Category = "Decoder",
		meta = (ClampMin = "30.", ClampMax = "100.", EditCondition = "bIsEncoded"))
	float EncodeTargetA;
	UPROPERTY(EditDefaultsOnly, Category = "Decoder", 
		meta = (ClampMin = "30.", ClampMax = "100.", EditCondition = "bIsEncoded"))
	float EncodeTargetB;
};
