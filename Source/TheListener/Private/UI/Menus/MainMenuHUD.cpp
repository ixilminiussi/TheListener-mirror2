// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/MainMenuHUD.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "UI/Menus/SettingsMenuWidget.h"
#include "UI/Menus/MainMenuWidget.h"

void AMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetOwningPlayerController();

	PC->SetInputMode(FInputModeGameOnly());

	if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
	{
		if (UCommonInputSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UCommonInputSubsystem>(LocalPlayer))
		{
			InputSubsystem->OnInputMethodChangedNative.AddUObject(this, &AMainMenuHUD::HandleInputMethodChanged);
		}
	}

	if (ensure(PreviousWidgetClass) && ensure(PC))
	{
		PreviousWidgetInstance = CreateWidget<UBaseMenuWidget>(PC, PreviousWidgetClass);

		if (ensure(PreviousWidgetInstance))
		{
			PreviousWidgetInstance->AddToViewport();
		}
	}
	if (ensure(SettingsMenuWidgetClass) && ensure(PC))
	{
		SettingsMenuWidgetInstance = CreateWidget<USettingsMenuWidget>(PC, SettingsMenuWidgetClass);
	}
}


void AMainMenuHUD::HandleInputMethodChanged(ECommonInputType NewInputType) const
{
	switch (NewInputType)
	{
	case ECommonInputType::Gamepad:
		PreviousWidgetInstance->GetFocusedButton()->SetFocus();

		break;

	case ECommonInputType::MouseAndKeyboard:
		break;

	default:
		break;
	}
}

USettingsMenuWidget* AMainMenuHUD::GetSettingsMenuWidget() const
{
	return SettingsMenuWidgetInstance;
}

UBaseMenuWidget* AMainMenuHUD::GetPreviousWidget()
{
	return PreviousWidgetInstance;
}
