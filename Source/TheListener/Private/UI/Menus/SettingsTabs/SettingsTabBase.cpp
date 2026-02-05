// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/SettingsTabs/SettingsTabBase.h"

#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"

void USettingsTabBase::NativeOnActivated()
{
	Super::NativeOnActivated();

	UWidget* WidgetToFocus = ScrollBox->GetChildAt(0);
	if (WidgetToFocus)
	{
		WidgetToFocus->SetFocus();
	}
}

void USettingsTabBase::NativeConstruct()
{
	Super::NativeConstruct();
}

void USettingsTabBase::SetupSettingsValues(USettingsSave* SettingSave)
{
}

class UScrollBox* USettingsTabBase::GetScrollBox() const
{
	return ScrollBox;
}
