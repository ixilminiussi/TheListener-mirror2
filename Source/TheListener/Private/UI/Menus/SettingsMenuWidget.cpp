// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/SettingsMenuWidget.h"

#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Menus/MainMenuHUD.h"
#include "UI/Menus/MainMenuWidget.h"

USwitcherTabSettings* USettingsMenuWidget::GetSwitcherTabSettings() const
{
	return SwitcherTabSettings;
}

void USettingsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply USettingsMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Right)
	{
		Return();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void USettingsMenuWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	SwitcherTabSettings->SetFocusOnElement(0.01f);
	Super::NativeOnFocusLost(InFocusEvent);
}

bool USettingsMenuWidget::NativeOnHandleBackAction()
{
	Return();
	return Super::NativeOnHandleBackAction();
}

void USettingsMenuWidget::Return() const
{
	const APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ABaseHUD* HUD = Cast<ABaseHUD>(PC->GetHUD());
	if (ensure(HUD))
	{
		HUD->CloseSettingsInBlueprint();
	}
}
