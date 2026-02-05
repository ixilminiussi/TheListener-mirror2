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

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Audio")
	TObjectPtr<class UAkAudioEvent> StartAudioEvent;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Audio")
	TObjectPtr<class UAkAudioEvent> StopAudioEvent;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<class UAkRtpc> Rtpc;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Audio",
		meta = (ClampMin = "30.0", ClampMax = "30000.0", DisplayPrecision = "2"))
	float Frequency = .0f;

	UPROPERTY(EditDefaultsOnly, Category = "Audio",
		meta = (ClampMin = "30.0", ClampMax = "30000.0", DisplayPrecision = "2"))
	float ReceptionBand = 70.f;

	TStaticArray<struct FKeyHandle, 3> CurveKeys{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<class UDialogueGraphAsset> DialogueAsset = nullptr;

	/** This variable will work only if you are using a class that is decodable (currently : SBP_Decoder */
	UPROPERTY(EditDefaultsOnly, Category = "Decoder")
	bool bIsEncoded = false;

	UPROPERTY(EditDefaultsOnly, Category = "Decoder",
		meta = (ClampMin = "30.", ClampMax = "100.", EditCondition = "bIsEncoded"))
	float EncodeTargetA = 50;
	UPROPERTY(EditDefaultsOnly, Category = "Decoder", 
		meta = (ClampMin = "30.", ClampMax = "100.", EditCondition = "bIsEncoded"))
	float EncodeTargetB = 50;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SSTV")
	TSubclassOf<AActor> SSTVClass = nullptr;
};
