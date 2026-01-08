#pragma once

#include "CoreMinimal.h"
#include "AkGameplayTypes.h"
#include "GameFramework/Actor.h"
#include "GPE/Dialogue.h"
#include "Station.generated.h"

#define INVALID_ID -1

struct FAnswer;

UCLASS()
class THELISTENER_API AStation : public ADialogue
{
	GENERATED_BODY()

	// Station ================================
public:
	friend class FCogGameplayWindow_Utilities;

	AStation();
	virtual void Destroyed() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Station")
	void StartStation();
	UFUNCTION(BlueprintCallable, Category = "Station")
	void StopStation();
	void SetClarity(float InSubtitleClarity, float InAudioClarity);
	UFUNCTION(BlueprintCallable, Category = "Station")
	float ComputeRawClarity(const float InFrequency) const;

	float SubtitleClarity, AudioClarity; 
	float MaxClarity = 1.0;

	virtual void SetupStation(class UStationAsset* InStationDataAsset, class ARadio* RadioActor);
	class UStationAsset* GetStationData() const;
	float GetFrequency() const;

	UFUNCTION(BlueprintCallable, Category = "Station")
	bool IsInRange(float InFrequency) const;
	void EnterRange();
	void LeaveRange();

	UFUNCTION()
	void Decode();

	friend class FCogGameplayWindow_Utilities;

protected:
	virtual bool IsHeard() override;
	bool bIsHeard = false;

	void StartDecoder();
	void StopDecoder();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Station")
	void OnDecoded();
	virtual void OnDecoded_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Station")
	void OnEnteringStationRange();
	virtual void OnEnteringStationRange_Implementation();
	UFUNCTION(BlueprintNativeEvent, Category = "Station")
	void OnLeavingStationRange();
	virtual void OnLeavingStationRange_Implementation();
	
public:
	UFUNCTION(BlueprintCallable, Category = "Station")
	int32 GetPlayPosition() const;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Station")
	TObjectPtr<class UStationAsset> StationAsset = nullptr;
	
	UPROPERTY()
	TObjectPtr<class ARadio> Radio = nullptr;
	// Station End ============================

	// Subtitles ==============================
protected:
	virtual void HandleMarker(FString const& Marker) override;
	void UpdateSubtitlesClarity(float Clarity);
	// Subtitles End ==========================

	// Dialogue ==============================
public:
	bool bWaitingForPromptCheck = false;
	
protected:
	bool bEarlyDialogueFlag = false;
	
	virtual void OnEndDialogue_Implementation() override;
	virtual void OnAnswerQuestion_Implementation() override;

	virtual void OnFinishDialogue() override;
};
