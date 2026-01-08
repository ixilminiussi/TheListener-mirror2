// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/UIElements/SwitcherTabSettings.h"

#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "UI/Menus/SettingsTabs/SettingsTabBase.h"
#include "UI/Menus/UIElements/GameSettingsSubsytem.h"
#include "UI/Menus/UIElements/WidgetTab.h"

void USwitcherTabSettings::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply USwitcherTabSettings::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Gamepad_RightShoulder)
	{
		ToRightTab();
		return FReply::Handled();
	}
	if (InKeyEvent.GetKey() == EKeys::Gamepad_LeftShoulder)
	{
		ToLeftTab();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void USwitcherTabSettings::SetIndex(int Index, float WaitingTime) const
{
	UWidgetTab* CurrentWidget = Cast<UWidgetTab>(TabHeaderBox->GetChildAt(Index));

	AnimatedSwitcher->SetActiveWidgetIndex(Index);

	for (UWidget* Widget : TabHeaderBox->GetAllChildren())
	{
		if (UWidgetTab* WidgetTab = Cast<UWidgetTab>(Widget))
		{
			WidgetTab->Toggle(false);
		}
	}
	check(CurrentWidget);
	CurrentWidget->Toggle(true);

	SetFocusOnElement(WaitingTime);
}


UHorizontalBox* USwitcherTabSettings::GetTabHeaderBox() const
{
	return TabHeaderBox;
}


void USwitcherTabSettings::ToRightTab() const
{
	const int32 NewIndex = (AnimatedSwitcher->GetActiveWidgetIndex() + TabHeaderBox->GetChildrenCount() + 1) %
		TabHeaderBox->GetChildrenCount();
	UCommonActivatableWidget* NewWidget = Cast<UCommonActivatableWidget>(AnimatedSwitcher->GetActiveWidget());
	NewWidget->ActivateWidget();
	SetIndex(NewIndex, 0.25f);
}

void USwitcherTabSettings::ToLeftTab() const
{
	const int32 NewIndex = (AnimatedSwitcher->GetActiveWidgetIndex() + TabHeaderBox->GetChildrenCount() - 1) %
		TabHeaderBox->GetChildrenCount();
	UCommonActivatableWidget* NewWidget = Cast<UCommonActivatableWidget>(AnimatedSwitcher->GetActiveWidget());
	NewWidget->ActivateWidget();
	SetIndex(NewIndex, 0.25f);
}

void USwitcherTabSettings::SetFocusOnElement(float WaitingTime) const
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	PC->FlushPressedKeys();

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(
		Handle,
		[this]()
		{
			USettingsTabBase* CurrentWidget = Cast<USettingsTabBase>(AnimatedSwitcher->GetActiveWidget());
			if (!CurrentWidget)
			{
				return;
			}
			CurrentWidget->GetVerticalBox()->GetChildAt(0)->SetFocus();
		},
		WaitingTime,
		false
	);
}

USettingsTabBase* USwitcherTabSettings::GetGameplayTab() const
{
	return GameplayTab;
}

USettingsTabBase* USwitcherTabSettings::GetAudioTab() const
{
	return AudioTab;
}

USettingsTabBase* USwitcherTabSettings::GetGraphicsTab() const
{
	return GraphicsTab;
}

USettingsTabBase* USwitcherTabSettings::GetAccessibilityTab() const
{
	return AccessibilityTab;
}

UWidgetTab* USwitcherTabSettings::GetGameplayTabHeader() const
{
	return GameplayTabHeader;
}

UWidgetTab* USwitcherTabSettings::GetAudioTabHeader() const
{
	return AudioTabHeader;
}

UWidgetTab* USwitcherTabSettings::GetGraphicsTabHeader() const
{
	return GraphicsTabHeader;
}

UWidgetTab* USwitcherTabSettings::GetAccessibilityTabHeader() const
{
	return AccessibilityTabHeader;
}


class UCommonAnimatedSwitcher* USwitcherTabSettings::GetAnimatedSwitcher() const
{
	return AnimatedSwitcher;
}
