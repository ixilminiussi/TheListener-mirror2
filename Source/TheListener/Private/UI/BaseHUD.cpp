// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BaseHUD.h"

#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Menus/SettingsMenuWidget.h"


void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();

	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	Settings->SetVSyncEnabled(true);
	Settings->ApplySettings(false);
}

UBaseMenuWidget* ABaseHUD::GetPreviousWidget()
{
	return nullptr;
}

USettingsMenuWidget* ABaseHUD::GetSettingsMenuWidget() const
{
	return nullptr;
}
