// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueLineDataFuture.h"
#include "EventGraphRuntime/Public/EventGraphConditionData.h"

TArray<UAnswer*> const &UDialogueLineData::GetAnswers() const
{
	return Next;
}

bool UDialogueLineData::IsAutoChoice() const
{
	return bIsAuto;
}

float UDialogueLineData::GetDuration() const
{
	return Duration;
}

uint32 UDialogueLineData::GetDefaultAnswer() const
{
	return DefaultAnswer;
}

bool UDialogueLineData::IsConditionValid()
{
	return ConditionData != nullptr ? ConditionData->IsValidated() : true;
}

#if WITH_EDITOR
void UDialogueLineData::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	OnDataChanged.Broadcast();
	UObject::PostEditChangeProperty(PropertyChangedEvent);
}
#endif
