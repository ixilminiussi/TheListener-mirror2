// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Menus/UIElements/ButtonPrimary.h"
#include "UI/Menus/UIElements/SwitcherTabSettings.h"
#include "CommonActivatableWidget.h"
#include "SettingsMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API USettingsMenuWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	USwitcherTabSettings* GetSwitcherTabSettings() const;

	virtual void NativeConstruct() override;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;


protected:
	UPROPERTY(meta = (BindWidget))
	USwitcherTabSettings* SwitcherTabSettings;

	UFUNCTION()
	void Return() const;
};
