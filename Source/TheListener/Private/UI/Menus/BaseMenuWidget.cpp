// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/BaseMenuWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/VerticalBox.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "System/Core/BaseGameInstance.h"
#include "UI/BaseHUD.h"
#include "UI/LukaHUD.h"
#include "UI/Menus/SettingsMenuWidget.h"
#include "UI/Menus/SettingsSave.h"
#include "UI/Menus/SettingsTabs/SettingsTabBase.h"
#include "UI/Menus/UIElements/ButtonPrimary.h"

UButtonPrimary* UBaseMenuWidget::GetFocusedButton() const
{
	return nullptr;
}

void UBaseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (ensure(PC))
	{
		PC->bEnableClickEvents = false;
		PC->bEnableTouchEvents = false;
	}

	check(ChangeLevelButton);
	ChangeLevelButton->OnClicked().AddUObject(this, &UBaseMenuWidget::OnChangeLevelClicked);

	check(SettingsButton)
	SettingsButton->OnClicked().AddUObject(this, &UBaseMenuWidget::OnSettingsClicked);

	check(QuitButton);
	QuitButton->OnClicked().AddUObject(this, &UBaseMenuWidget::OnQuitClicked);

	check(YesQuitButton);
	YesQuitButton->OnClicked().AddUObject(this, &UBaseMenuWidget::OnYesQuitClicked);

	check(NoQuitButton);
	NoQuitButton->OnClicked().AddUObject(this, &UBaseMenuWidget::OnNoQuitClicked);


	check(QuitPanel);
	QuitPanel->SetVisibility(ESlateVisibility::Collapsed);
}

class UButton* UBaseMenuWidget::GetPreMenuButton()
{
	return nullptr;
}

void UBaseMenuWidget::OnChangeLevelClicked_Implementation()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), NewLevel);
}

void UBaseMenuWidget::OnSettingsClicked() const
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ABaseHUD* HUD = Cast<ABaseHUD>(PC->GetHUD()))
		{
			if (USettingsMenuWidget* Settings = HUD->GetSettingsMenuWidget())
			{
				if (Settings)
				{
					Settings->AddToViewport(TOP_LEVEL + 1);
				}
				HUD->GetPreviousWidget()->SetVisibility(ESlateVisibility::Collapsed);
				Settings->GetSwitcherTabSettings()->SetIndex(0, 0.02f);
				Settings->SetVisibility(ESlateVisibility::Visible);
				HUD->GetPreviousWidget()->SetIsEnabled(false);

				// SAVE
				UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
				check(GI);
				USettingsSave* SettingsSave = GI->GetSettingsSave();
				check(SettingsSave);
				USettingsTabBase* GameplayTab = Settings->GetSwitcherTabSettings()->GetGameplayTab();
				check(GameplayTab);
				GameplayTab->SetupSettingsValues(SettingsSave);
			}
		}
	}
}

void UBaseMenuWidget::OnQuitClicked() const
{
	MainPanel->SetVisibility(ESlateVisibility::Collapsed);
	QuitPanel->SetVisibility(ESlateVisibility::Visible);
	YesQuitButton->SetFocus();
}

void UBaseMenuWidget::OnYesQuitClicked() const
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

void UBaseMenuWidget::OnNoQuitClicked() const
{
	QuitPanel->SetVisibility(ESlateVisibility::Collapsed);
	MainPanel->SetVisibility(ESlateVisibility::Visible);
	GetFocusedButton()->SetFocus();
}
