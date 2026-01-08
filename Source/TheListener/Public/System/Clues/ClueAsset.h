// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ClueAsset.generated.h"

/**
 * 
 */
UENUM()
enum class EClueState : uint8
{
	Hidden,
	Partial,
	Complete,
	Disabled,
	Missing
};

UENUM()
enum class EClueDisplayType : uint8
{
	None,
	Picture,
	Text
};

UCLASS()
class THELISTENER_API UClueAsset : public UDataAsset
{
	GENERATED_BODY()

#if UE_EDITOR

protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	UPROPERTY()
	EClueState State;

	UPROPERTY(EditAnywhere)
	EClueDisplayType DisplayType;

	UPROPERTY(EditAnywhere)
	FString Name;

	UPROPERTY(EditAnywhere)
	FString Desc;

	UPROPERTY(EditAnywhere)
	UTexture2D* Image;

	UPROPERTY(EditAnywhere)
	FString ShortDesc;

	UPROPERTY(EditAnywhere)
	TArray<class UClueAsset*> Links;

	UPROPERTY()
	TArray<class UClueAsset*> PreviousLinks;

	void ResetClueState();

	void CheckClueState(bool hasBeenFound);

	void MakeUnavailable();

	bool IsFound() const;
};
