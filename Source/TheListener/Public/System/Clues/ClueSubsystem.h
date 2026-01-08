// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClueSubsystem.generated.h"

UCLASS()
class THELISTENER_API UClueSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void OnActorsInitialized(const FActorsInitializedParams& Params);
	void InitCorkboard();

	void GiveClue(class UClueAsset* Clue);
	void RemoveClue(class UClueAsset* Clue);

protected:
	TObjectPtr<class ACorkboard> Corkboard;

	TArray<class UClueAsset*> CluesList;
	TArray<class UClueAsset*> FoundClues;

#if UE_EDITOR
	void DisplayClues();
#endif
};
