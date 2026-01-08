// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/SettingsTabs/SettingsGameplayWidget.h"

#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "System/Core/BaseGameInstance.h"
#include "UI/Menus/SettingsSave.h"
#include "UI/Menus/SettingsTabs/SettingsAudioWidget.h"
#include "UI/Menus/UIElements/CheckBoxSettings.h"
#include "UI/Menus/UIElements/SliderSettings.h"

void USettingsGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);
	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	check(VerticalSensibilitySlider);
	VerticalSensibilitySlider->SetNumberSlider(SettingsSave->GetVerticalSensitivity());
	VerticalSensibilitySlider->GetSlider()->OnValueChanged.AddDynamic(
		this, &USettingsGameplayWidget::OnVerticalSensitivitySliderValueChanged);

	check(HorizontalSensibilitySlider);
	HorizontalSensibilitySlider->SetNumberSlider(SettingsSave->GetHorizontalSensitivity());
	HorizontalSensibilitySlider->GetSlider()->OnValueChanged.AddDynamic(
		this, &USettingsGameplayWidget::OnHorizontalSensitivitySliderValueChanged);

	check(ControllerHapticFeedback);
	ControllerHapticFeedback->GetCheckBox()->SetIsChecked(SettingsSave->GetControllerHapticFeedback());
	ControllerHapticFeedback->GetCheckBox()->OnCheckStateChanged.AddDynamic(
		this, &USettingsGameplayWidget::OnControllerHapticFeedbackChanged);
}

void USettingsGameplayWidget::SetupSettingsValues(USettingsSave* SettingSave)
{
	VerticalSensibilitySlider->GetSlider()->SetValue(SettingSave->GetVerticalSensitivity());
	HorizontalSensibilitySlider->GetSlider()->SetValue(SettingSave->GetHorizontalSensitivity());
	ControllerHapticFeedback->GetCheckBox()->SetIsChecked(SettingSave->GetControllerHapticFeedback());
}

void USettingsGameplayWidget::OnVerticalSensitivitySliderValueChanged(float Value)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	SettingsSave->SetVerticalSensitivity(Value);
}

void USettingsGameplayWidget::OnHorizontalSensitivitySliderValueChanged(float Value)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	SettingsSave->SetHorizontalSensitivity(Value);
}

void USettingsGameplayWidget::OnControllerHapticFeedbackChanged(bool bValue)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	SettingsSave->SetControllerHapticFeedback(bValue);
	ControllerHapticFeedback->GetCheckBox()->SetIsChecked(bValue);
}
