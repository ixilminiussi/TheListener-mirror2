// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../../Wwise/Source/AkAudio/Classes/AkAudioEvent.h"
#include "UObject/Object.h"
#include "DialogueLineData.generated.h"

USTRUCT(BlueprintType)
struct FAnswer
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	FString Text;
	UPROPERTY(EditDefaultsOnly)
	int Next = 0;
};

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DIALOGUEGRAPHRUNTIME_API UDialogueLineData : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, meta=(AllowClear=true))
	TArray<FAnswer> Next{};
	UPROPERTY(EditDefaultsOnly) // If timer runs out, which answer is selected ##!Auto only
	uint32 DefaultAnswer = 0;
	UPROPERTY(EditDefaultsOnly) // How long the player has to reply ##!Auto only
	float Duration = -1.f;
	UPROPERTY(EditDefaultsOnly)
	bool bIsAuto = false;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAkAudioEvent> StartAudioEvent{nullptr};
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAkAudioEvent> StopAudioEvent{nullptr};
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UEventGraphBaseConditionData> ConditionData{nullptr};

public:
	TArray<FAnswer> const &GetAnswers() const;
	bool IsAutoChoice() const;
	float GetDuration() const;
	uint32 GetDefaultAnswer() const;
	TObjectPtr<class UAkAudioEvent> GetStartAudioEvent() {return StartAudioEvent;}
	TObjectPtr<class UAkAudioEvent> GetStopAudioEvent() {return StopAudioEvent;}

	bool IsConditionValid();
};

UCLASS(BlueprintType)
class UDialogueTreeData : public UDataAsset
{
	GENERATED_BODY()

	public:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UDialogueLineData>> Nodes;

	UPROPERTY(EditDefaultsOnly)
	int Root = 0;
};