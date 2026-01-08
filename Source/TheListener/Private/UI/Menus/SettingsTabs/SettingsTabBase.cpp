// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/SettingsTabs/SettingsTabBase.h"

#include "CommonTextBlock.h"
#include "UI/Menus/UIElements/ButtonUIElement.h"

void USettingsTabBase::NativeConstruct()
{
	Super::NativeConstruct();
}

void USettingsTabBase::SetupSettingsValues(USettingsSave* SettingSave)
{
}

class UVerticalBox* USettingsTabBase::GetVerticalBox() const
{
	return VerticalBox;
}
