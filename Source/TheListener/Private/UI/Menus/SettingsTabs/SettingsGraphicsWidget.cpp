// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/SettingsTabs/SettingsGraphicsWidget.h"

#include "CommonAnimatedSwitcher.h"
#include "GameFramework/GameUserSettings.h"
#include "System/Core/BaseGameInstance.h"
#include "UI/Menus/SettingsSave.h"
#include "UI/Menus/UIElements/DropdownSettings.h"

void USettingsGraphicsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);
	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	check(ResolutionDropdown);
	ResolutionDropdown->GetSwitcher()->SetActiveWidgetIndex(SettingsSave->GetResolutionDropdownIndex());
	ResolutionDropdown->GetSwitcher()->OnActiveWidgetIndexChanged.AddUObject(
		this, &USettingsGraphicsWidget::OnResolutionDropdownValueChanged);

	check(DisplayDropdown);
	DisplayDropdown->GetSwitcher()->SetActiveWidgetIndex(SettingsSave->GetDisplayDropdownIndex());
	DisplayDropdown->GetSwitcher()->OnActiveWidgetIndexChanged.AddUObject(
		this, &USettingsGraphicsWidget::OnDisplayDropdownValueChanged);
}

void USettingsGraphicsWidget::SetupSettingsValues(class USettingsSave* SettingSave)
{
	ResolutionDropdown->GetSwitcher()->SetActiveWidgetIndex(SettingSave->GetResolutionDropdownIndex());
	DisplayDropdown->GetSwitcher()->SetActiveWidgetIndex(SettingSave->GetDisplayDropdownIndex());
	Super::SetupSettingsValues(SettingSave);
}

void USettingsGraphicsWidget::OnFramerateDropdownValueChanged(FText Text)
{
}

void USettingsGraphicsWidget::OnResolutionDropdownValueChanged(UWidget* Widget, int32 Index)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);
	SettingsSave->SetResolutionDropdownIndex(Index);


	// TODO Set Resolution

	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	if (!GameUserSettings)
	{
		return;
	}

	GameUserSettings->SetScreenResolution(ResolutionDropdown->GetSelectedOption().IntPointValue);
}

void USettingsGraphicsWidget::OnDisplayDropdownValueChanged(UWidget* Widget, int32 Index)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);
	SettingsSave->SetResolutionDropdownIndex(Index);


	// TODO Set Display

	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	if (!GameUserSettings)
	{
		return;
	}
	GameUserSettings->SetFullscreenMode(DisplayDropdown->GetSelectedOption().WindowModeValue);
}

void USettingsGraphicsWidget::OnQualityPresetDropdownValueChanged(FText Text)
{
}
