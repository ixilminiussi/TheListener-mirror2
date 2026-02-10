// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SettingsTabBase.h"
#include "SettingsAccessibilityWidget.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API USettingsAccessibilityWidget : public USettingsTabBase
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	virtual void SetupSettingsValues(class USettingsSave* SettingSave) override;

	protected:
	UPROPERTY(meta = (BindWidget))
	class UDropdownSettings* LanguageDropdown;
	UPROPERTY(meta = (BindWidget))
	class USliderSettings* SubtitlesBackgroundSlider;
	UPROPERTY(meta = (BindWidget))
	class USliderSettings* SubtitlesSizeSlider;
	UPROPERTY(meta = (BindWidget))
	class UCheckBoxSettings* HardOfEaringModCheckbox;

	UFUNCTION()
	void OnLanguageDropdownValueChanged(UWidget* Widget, int32 Index);
	UFUNCTION()
	void OnSubtitlesBackgroundOpacityChanged(float Value);
	UFUNCTION()
	void OnSubtitlesSizeChanged(float Value);
	UFUNCTION()
	void OnHardOfEaringModChanged(bool bValue);
};
