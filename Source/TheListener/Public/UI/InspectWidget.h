// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "InspectWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectorUse,FString,SelectorName);

/**
 * 
 */
UCLASS()
class THELISTENER_API UInspectWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Toy|Files|Selector")
	bool bHasSelector = false;

	// BEGIN - Widgets
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Files|UI", meta = (BindWidget))
	class UCanvasPanel* ContentCanvasPanel;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Files|UI", meta = (BindWidget))
	class UImage* SelectorImage;

	TArray<class USelectorBox*> SelectorBoxes;

	TMap<FString, TSet<class USelectorBox*>> SelectorBoxesMap;
	// END - Widgets

	//BEGIN - Controls
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Files|Controls")
	float MoveSpeed = 10.0f;
	//END - Controls

	float CurrentSelectorSize = 0.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Files|Controls")
	float SelectorSizeMultiplier = 1.0f;
	

public:

	USelectorBox* CheckSelectorBoxes(FVector2D SelectorPosition);

	void MoveSelector(FVector2D Value);
	void Select();

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintAssignable)
	FOnSelectorUse OnSelectorUse;
};
