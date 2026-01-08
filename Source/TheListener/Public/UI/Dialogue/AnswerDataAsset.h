#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DialogueGraphRuntime/Public/DialogueLineData.h"
#include "AnswerDataAsset.generated.h"

class UConstantQNRT;
class UAkSwitchValue;

USTRUCT(BlueprintType)
struct FAnswerList
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FAnswer> Answers;
	UPROPERTY(EditDefaultsOnly)
	uint32 DefaultAnswer = 0;
	UPROPERTY(EditDefaultsOnly)
	float Duration = -1.f;
};

inline FAnswerList GetAnswerList(UDialogueLineData *Data)
{
	FAnswerList Answers{};

	if (Data->IsAutoChoice())
	{
		return Answers;
	}

	Answers.DefaultAnswer = Data->GetDefaultAnswer();
	Answers.Duration = Data->GetDuration();
	Answers.Answers = Data->GetAnswers(); 

	return Answers;
}