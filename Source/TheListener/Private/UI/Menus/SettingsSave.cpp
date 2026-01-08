// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/SettingsSave.h"

#include "AkGameplayStatics.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Menus/SettingsDataAsset.h"

void USettingsSave::InitializeFromDefaults(USettingsDataAsset* DefaultData)
{
	if (!ensure(DefaultData))
	{
		return;
	}
	//Gameplay
	HorizontalSensitivity = DefaultData->HorizontalSensitivity;
	VerticalSensitivity = DefaultData->VerticalSensitivity;
	//Audio
	MasterVolume = DefaultData->MasterVolume;
	RadioStaticVolume = DefaultData->RadioStaticVolume;
	MusicVolume = DefaultData->MusicVolume;
	RadioVolume = DefaultData->RadioVolume;
	SFXVolume = DefaultData->SFXVolume;
	LukaVolume = DefaultData->LukaVolume;
	AmbientVolume = DefaultData->AmbientVolume;
	//Graphics
	ResolutionDropdownIndex = DefaultData->ResolutionIndex;
	DisplayDropdownIndex = DefaultData->DisplayIndex;

	UGameplayStatics::SaveGameToSlot(this, TEXT("Settings"), 0);
}

void USettingsSave::SetHorizontalSensitivity(float Value)
{
	HorizontalSensitivity = Value;
	OnSettingsChanged.Broadcast();
	UGameplayStatics::SaveGameToSlot(this, TEXT("Settings"), 0);
}

void USettingsSave::SetVerticalSensitivity(float Value)
{
	VerticalSensitivity = Value;
	OnSettingsChanged.Broadcast();
	UGameplayStatics::SaveGameToSlot(this, TEXT("Settings"), 0);
}

void USettingsSave::SetControllerHapticFeedback(bool bValue)
{
	ControllerHapticFeedback = bValue;
	OnSettingsChanged.Broadcast();
	UGameplayStatics::SaveGameToSlot(this, TEXT("Settings"), 0);
}

void USettingsSave::SetResolutionDropdownIndex(int32 Index)
{
	ResolutionDropdownIndex = Index;
	OnSettingsChanged.Broadcast();
	UGameplayStatics::SaveGameToSlot(this, TEXT("Settings"), 0);
}

void USettingsSave::SetDisplayDropdownIndex(int32 Index)
{
	DisplayDropdownIndex = Index;
	OnSettingsChanged.Broadcast();
	UGameplayStatics::SaveGameToSlot(this, TEXT("Settings"), 0);
}

void USettingsSave::SetMasterVolume(float Value)
{
	MasterVolume = Value;
	OnSettingsChanged.Broadcast();
	UGameplayStatics::SaveGameToSlot(this, TEXT("Settings"), 0);
}

void USettingsSave::SetRadioStaticVolume(float Value)
{
	RadioStaticVolume = Value;
	OnSettingsChanged.Broadcast();
	UGameplayStatics::SaveGameToSlot(this, TEXT("Settings"), 0);
}

void USettingsSave::SetMusicVolume(float Value)
{
	MusicVolume = Value;
	OnSettingsChanged.Broadcast();
	UGameplayStatics::SaveGameToSlot(this, TEXT("Settings"), 0);
}

void USettingsSave::SetRadioVolume(float Value)
{
	RadioVolume = Value;
	OnSettingsChanged.Broadcast();
	UGameplayStatics::SaveGameToSlot(this, TEXT("Settings"), 0);
}

void USettingsSave::SetSFXVolume(float Value)
{
	SFXVolume = Value;
	OnSettingsChanged.Broadcast();
	UGameplayStatics::SaveGameToSlot(this, TEXT("Settings"), 0);
}

void USettingsSave::SetLukaVolume(float Value)
{
	LukaVolume = Value;
	OnSettingsChanged.Broadcast();
	UGameplayStatics::SaveGameToSlot(this, TEXT("Settings"), 0);
}

void USettingsSave::SetAmbientVolume(float Value)
{
	AmbientVolume = Value;
	OnSettingsChanged.Broadcast();
	UGameplayStatics::SaveGameToSlot(this, TEXT("Settings"), 0);
}

float USettingsSave::GetHorizontalSensitivity() const
{
	return HorizontalSensitivity;
}

float USettingsSave::GetVerticalSensitivity() const
{
	return VerticalSensitivity;
}

bool USettingsSave::GetControllerHapticFeedback() const
{
	return ControllerHapticFeedback;
}

int32 USettingsSave::GetResolutionDropdownIndex() const
{
	return ResolutionDropdownIndex;
}

int32 USettingsSave::GetDisplayDropdownIndex() const
{
	return DisplayDropdownIndex;
}

float USettingsSave::GetMasterVolume() const
{
	return MasterVolume;
}

float USettingsSave::GetRadioStaticVolume() const
{
	return RadioStaticVolume;
}

float USettingsSave::GetMusicVolume() const
{
	return MusicVolume;
}

float USettingsSave::GetRadioVolume() const
{
	return RadioVolume;
}

float USettingsSave::GetSFXVolume() const
{
	return SFXVolume;
}

float USettingsSave::GetLukaVolume() const
{
	return LukaVolume;
}

float USettingsSave::GetAmbientVolume() const
{
	return AmbientVolume;
}
