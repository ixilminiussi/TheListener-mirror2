// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BaseHUD.h"

#include "UI/Menus/SettingsMenuWidget.h"


void ABaseHUD::BeginPlay()
{
	Super::BeginPlay();
}

UBaseMenuWidget* ABaseHUD::GetPreviousWidget()
{
	return nullptr;
}

USettingsMenuWidget* ABaseHUD::GetSettingsMenuWidget() const
{
	return nullptr;
}
