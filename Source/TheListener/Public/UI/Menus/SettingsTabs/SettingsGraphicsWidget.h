// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SettingsTabBase.h"
#include "SettingsGraphicsWidget.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API USettingsGraphicsWidget : public USettingsTabBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void SetupSettingsValues(class USettingsSave* SettingSave) override;

protected:
	UPROPERTY(meta = (BindWidget))
	class UDropdownSettings* FramerateDropdown;
	UPROPERTY(meta = (BindWidget))
	class UDropdownSettings* ResolutionDropdown;
	UPROPERTY(meta = (BindWidget))
	class UDropdownSettings* DisplayDropdown;
	UPROPERTY(meta = (BindWidget))
	class UDropdownSettings* QualityPresetDropdown;

	UFUNCTION()
	void OnFramerateDropdownValueChanged(FText Text);

	UFUNCTION()
	void OnResolutionDropdownValueChanged(UWidget* Widget, int32 Index);

	UFUNCTION()
	void OnDisplayDropdownValueChanged(UWidget* Widget, int32 Index);

	UFUNCTION()
	void OnQualityPresetDropdownValueChanged(FText Text);
};
