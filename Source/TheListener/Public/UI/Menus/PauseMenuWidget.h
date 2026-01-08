// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMenuWidget.h"
#include "PauseMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API UPauseMenuWidget : public UBaseMenuWidget
{
	GENERATED_BODY()

public:
	virtual UButtonPrimary* GetFocusedButton() const override;

	virtual void NativeConstruct() override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// Buttons Functions
	UFUNCTION()
	void OnResumeClicked() const;


	virtual void OnSettingsClicked() const override;

	virtual void OnChangeLevelClicked_Implementation() override;

	virtual void OnQuitClicked() const override;


	// Widgets Buttons
	UPROPERTY(meta = (BindWidget))
	UButtonPrimary* ResumeButton;
};
