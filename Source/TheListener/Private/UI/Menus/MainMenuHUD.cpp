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
}

USettingsMenuWidget* AMainMenuHUD::GetSettingsMenuWidget() const
{
	return SettingsMenuWidgetInstance;
}

UBaseMenuWidget* AMainMenuHUD::GetPreviousWidget()
{
	return PreviousWidgetInstance;
}
