// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../../Wwise/Source/AkAudio/Classes/AkAudioEvent.h"
#include "UObject/Object.h"
#include "DialogueLineDataFuture.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class UAnswer : public UObject
{
	GENERATED_BODY()
	public:
	UPROPERTY(EditDefaultsOnly)
	FString Text;
	UPROPERTY()
	UDialogueLineData* NextLine;
	UPROPERTY()
	FGuid PinGuid{};
};

UENUM()
enum class LineType : uint8
{
	Answer = 0,
	LeaveNode = 1
};


#if WITH_EDITOR
DECLARE_MULTICAST_DELEGATE(FOnDataChanged)
#endif

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class DIALOGUEGRAPHRUNTIME_API UDialogueLineData : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	LineType Type = LineType::Answer;
	UPROPERTY()
	TArray<UAnswer*> Next{};
	UPROPERTY(EditDefaultsOnly) // If timer runs out, which answer is selected ##!Auto only
	uint32 DefaultAnswer = 0;
	UPROPERTY(EditDefaultsOnly) // How long the player has to reply ##!Auto only
	float Duration = -1.f;
	UPROPERTY()
	bool bIsAuto = false;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAkAudioEvent> PlayAudioEvent{nullptr};
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAkAudioEvent> StopAudioEvent{nullptr};
	UPROPERTY()
	TObjectPtr<class UEventGraphBaseConditionData> ConditionData{nullptr};
	
	TArray<UAnswer*> const &GetAnswers() const;
	bool IsAutoChoice() const;
	float GetDuration() const;
	uint32 GetDefaultAnswer() const;
	TObjectPtr<class UAkAudioEvent> GetPlayAudioEvent() {return PlayAudioEvent;}
	TObjectPtr<class UAkAudioEvent> GetStopAudioEvent() {return StopAudioEvent;}

	bool IsConditionValid();

	UPROPERTY()
	bool IsStart = false;

	#if WITH_EDITOR
	public:
	FOnDataChanged OnDataChanged;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif
};

UCLASS()
class DIALOGUEGRAPHRUNTIME_API UDialogueTreeData : public UObject
{
	GENERATED_BODY()
	public:
	UPROPERTY()
	TArray<UDialogueLineData*> AllData{};
	UPROPERTY()
	UDialogueLineData* StartLine = nullptr;
};