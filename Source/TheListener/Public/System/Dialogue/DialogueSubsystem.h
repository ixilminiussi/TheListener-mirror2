// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DialogueSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueEnd, class UInkpotStory*, Story);

/**
 * 
 */
UCLASS()
class THELISTENER_API UDialogueSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void OnActorsInitialized(const FActorsInitializedParams& Params);

	virtual void Deinitialize() override;

	void OnSelectChoice(uint32 ChoiceID, uint32 Choice);

	void SetText();

	void Continue();

	class UInkpotStory* StartStory(class UInkpotStoryAsset* NewStory);
	UFUNCTION()
	void OnStoryBegin(UInkpotStory* NewStory);
	UFUNCTION()
	void OnStoryContinue(UInkpotStory* Story);
	
	void FinishStory();

	class APhone* GetPhone();

	void CheckTags();

	FOnDialogueEnd EventOnDialogueEnd;

protected:
	UPROPERTY()
	TObjectPtr<class APhone> Phone{nullptr};

	UPROPERTY()
	TObjectPtr<class UInkpotStory> CurrentStory;

	uint32 CurrentChoiceID;
};
