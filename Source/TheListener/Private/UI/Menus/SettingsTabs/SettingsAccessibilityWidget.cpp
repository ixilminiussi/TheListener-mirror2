// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/SettingsTabs/SettingsAccessibilityWidget.h"

#include "CommonAnimatedSwitcher.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "System/Core/BaseGameInstance.h"
#include "UI/Menus/SettingsSave.h"
#include "UI/Menus/UIElements/CheckBoxSettings.h"
#include "UI/Menus/UIElements/DropdownSettings.h"
#include "UI/Menus/UIElements/SliderSettings.h"

void USettingsAccessibilityWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);
	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	check(LanguageDropdown);
	LanguageDropdown->GetSwitcher()->SetActiveWidgetIndex((SettingsSave->GetLangageIndex()));
	LanguageDropdown->GetSwitcher()->OnActiveWidgetIndexChanged.AddUObject(
		this, &USettingsAccessibilityWidget::OnLanguageDropdownValueChanged);

	check(SubtitlesBackgroundSlider);
	SubtitlesBackgroundSlider->SetNumberSlider(SettingsSave->GetSubtitlesBackgroundOpacityValue());
	SubtitlesBackgroundSlider->GetSlider()->OnValueChanged.AddDynamic(
		this, &USettingsAccessibilityWidget::OnSubtitlesBackgroundOpacityChanged);

	check(SubtitlesSizeSlider);
	SubtitlesSizeSlider->SetNumberSlider(SettingsSave->GetSubtitlesSizeValue());
	SubtitlesSizeSlider->GetSlider()->OnValueChanged.AddDynamic(
		this, &USettingsAccessibilityWidget::OnSubtitlesSizeChanged);

	check(HardOfEaringModCheckbox);
	HardOfEaringModCheckbox->GetCheckBox()->SetIsChecked(SettingsSave->GetHardOfEaringMod());
	HardOfEaringModCheckbox->GetCheckBox()->OnCheckStateChanged.AddDynamic(
		this, &USettingsAccessibilityWidget::OnHardOfEaringModChanged);
}

void USettingsAccessibilityWidget::SetupSettingsValues(class USettingsSave* SettingSave)
{
	LanguageDropdown->GetSwitcher()->SetActiveWidgetIndex(SettingSave->GetLangageIndex());
	SubtitlesBackgroundSlider->GetSlider()->SetValue(SettingSave->GetSubtitlesBackgroundOpacityValue());
	SubtitlesSizeSlider->GetSlider()->SetValue(SettingSave->GetSubtitlesSizeValue());
	HardOfEaringModCheckbox->GetCheckBox()->SetIsChecked(SettingSave->GetHardOfEaringMod());
	Super::SetupSettingsValues(SettingSave);
}

void USettingsAccessibilityWidget::OnLanguageDropdownValueChanged(UWidget* Widget, int32 Index)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);
	SettingsSave->SetLanguageDropdownIndex(Index);

	//Logique de changement de langue
}

void USettingsAccessibilityWidget::OnSubtitlesBackgroundOpacityChanged(float Value)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);
	SettingsSave->SetSubtitlesBackgroundOpacityValue(Value);
}

void USettingsAccessibilityWidget::OnSubtitlesSizeChanged(float Value)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);
	SettingsSave->SetSubtitlesSizeValue(Value);
}

void USettingsAccessibilityWidget::OnHardOfEaringModChanged(bool bValue)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);
	SettingsSave->SetHardOfEaringMod(bValue);
}
