// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/SettingsTabs/SettingsAudioWidget.h"

#include "AkRtpc.h"
#include "Components/Slider.h"
#include "System/Core/BaseGameInstance.h"
#include "UI/Menus/SettingsSave.h"
#include "UI/Menus/UIElements/SliderSettings.h"

void USettingsAudioWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(MasterSlider);

	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);
	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	MasterSlider->SetNumberSlider(SettingsSave->GetMasterVolume());
	MasterSlider->GetSlider()->OnValueChanged.AddDynamic(
		this, &USettingsAudioWidget::OnMasterSliderValueChanged);

	RadioStaticSlider->SetNumberSlider(SettingsSave->GetRadioStaticVolume());
	RadioStaticSlider->GetSlider()->OnValueChanged.AddDynamic(
		this, &USettingsAudioWidget::OnRadioStaticSliderValueChanged);

	MusicSlider->SetNumberSlider(SettingsSave->GetMusicVolume());
	MusicSlider->GetSlider()->OnValueChanged.AddDynamic(
		this, &USettingsAudioWidget::OnMusicSliderValueChanged);

	RadioSlider->SetNumberSlider(SettingsSave->GetRadioVolume());
	RadioSlider->GetSlider()->OnValueChanged.AddDynamic(
		this, &USettingsAudioWidget::OnRadioSliderValueChanged);

	SFXSlider->SetNumberSlider(SettingsSave->GetSFXVolume());
	SFXSlider->GetSlider()->OnValueChanged.AddDynamic(
		this, &USettingsAudioWidget::OnSFXSliderValueChanged);

	LukaSlider->SetNumberSlider(SettingsSave->GetLukaVolume());
	LukaSlider->GetSlider()->OnValueChanged.AddDynamic(
		this, &USettingsAudioWidget::OnLukaSliderValueChanged);

	AmbientSlider->SetNumberSlider(SettingsSave->GetAmbientVolume());
	AmbientSlider->GetSlider()->OnValueChanged.AddDynamic(
		this, &USettingsAudioWidget::OnAmbientSliderValueChanged);
}


void USettingsAudioWidget::OnMasterSliderValueChanged(float Value)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	SettingsSave->SetMasterVolume(Value);
}

void USettingsAudioWidget::OnRadioStaticSliderValueChanged(float Value)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	SettingsSave->SetRadioStaticVolume(Value);
}

void USettingsAudioWidget::OnMusicSliderValueChanged(float Value)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	SettingsSave->SetMusicVolume(Value);
}

void USettingsAudioWidget::OnRadioSliderValueChanged(float Value)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	SettingsSave->SetRadioVolume(Value);
}

void USettingsAudioWidget::OnSFXSliderValueChanged(float Value)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	SettingsSave->SetSFXVolume(Value);
}

void USettingsAudioWidget::OnLukaSliderValueChanged(float Value)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	SettingsSave->SetLukaVolume(Value);
}

void USettingsAudioWidget::OnAmbientSliderValueChanged(float Value)
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	SettingsSave->SetAmbientVolume(Value);
}


