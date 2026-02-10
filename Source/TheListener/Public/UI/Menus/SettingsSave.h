// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIElements/DropdownSettings.h"
#include "SettingsSave.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsChanged);

UCLASS()
class THELISTENER_API USettingsSave : public USaveGame
{
	GENERATED_BODY()

protected:
	// Runtime Values
	// Gameplay
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float HorizontalSensitivity;
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float VerticalSensitivity;
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	bool ControllerHapticFeedback = true;
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float ReticleSize;

	//Graphics
	UPROPERTY(EditDefaultsOnly, Category = "Graphics")
	int32 ResolutionDropdownIndex;
	UPROPERTY(EditDefaultsOnly, Category = "Graphics")
	int32 FramerateDropdownIndex;
	UPROPERTY(EditDefaultsOnly, Category = "Graphics")
	int32 DisplayDropdownIndex;

	//Audio
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float MasterVolume;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float RadioStaticVolume;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float MusicVolume;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float RadioVolume;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float SFXVolume;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float LukaVolume;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float AmbientVolume;

	//Accessibility
	UPROPERTY(EditDefaultsOnly, Category = "Accessibility")
	int32 LangageIndex;
	UPROPERTY(EditDefaultsOnly, Category = "Accessibility")
	float SubtitlesBackgroundOpacityValue;
	UPROPERTY(EditDefaultsOnly, Category = "Accessibility")
	float SubtitlesSizeValue;
	UPROPERTY(EditDefaultsOnly, Category = "Accessibility")
	bool HardOfEaringMod;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Events")
	FOnSettingsChanged OnSettingsChanged;

	// Events

	UFUNCTION()
	void InitializeFromDefaults(class USettingsDataAsset* DefaultData);

	// Setters
	// Gameplay
	UFUNCTION()
	void SetHorizontalSensitivity(float Value);
	UFUNCTION()
	void SetVerticalSensitivity(float Value);
	UFUNCTION()
	void SetControllerHapticFeedback(bool bValue);
	UFUNCTION()
	void SetReticleSize(float Value);

	//Graphics
	UFUNCTION()
	void SetResolutionDropdownIndex(int32 Index);
	UFUNCTION()
	void SetFramerateDropdownIndex(int32 Index);
	UFUNCTION()
	void SetDisplayDropdownIndex(int32 Index);

	// Audio
	UFUNCTION()
	void SetMasterVolume(float Value);
	UFUNCTION()
	void SetRadioStaticVolume(float Value);
	UFUNCTION()
	void SetMusicVolume(float Value);
	UFUNCTION()
	void SetRadioVolume(float Value);
	UFUNCTION()
	void SetSFXVolume(float Value);
	UFUNCTION()
	void SetLukaVolume(float Value);
	UFUNCTION()
	void SetAmbientVolume(float Value);

	//Accessibility
	UFUNCTION()
	void SetLanguageDropdownIndex(int32 Index);
	UFUNCTION()
	void SetSubtitlesBackgroundOpacityValue(float Value);
	UFUNCTION()
	void SetSubtitlesSizeValue(float Value);
	UFUNCTION()
	void SetHardOfEaringMod(bool bValue);

	// Getters
	// Gameplay
	UFUNCTION()
	float GetHorizontalSensitivity() const;
	UFUNCTION()
	float GetVerticalSensitivity() const;
	UFUNCTION(BlueprintCallable)
	bool GetControllerHapticFeedback() const;
	UFUNCTION()
	float GetReticleSize() const;

	//Graphics
	UFUNCTION()
	int32 GetResolutionDropdownIndex() const;
	UFUNCTION()
	int32 GetFramerateDropdownIndex() const;
	UFUNCTION()
	int32 GetDisplayDropdownIndex() const;

	// Audio
	UFUNCTION(BlueprintCallable)
	float GetMasterVolume() const;
	UFUNCTION(BlueprintCallable)
	float GetRadioStaticVolume() const;
	UFUNCTION(BlueprintCallable)
	float GetMusicVolume() const;
	UFUNCTION(BlueprintCallable)
	float GetRadioVolume() const;
	UFUNCTION(BlueprintCallable)
	float GetSFXVolume() const;
	UFUNCTION(BlueprintCallable)
	float GetLukaVolume() const;
	UFUNCTION(BlueprintCallable)
	float GetAmbientVolume() const;

	//Accessibility
	UFUNCTION(BlueprintCallable)
	int32 GetLangageIndex() const;
	UFUNCTION(BlueprintCallable)
	float GetSubtitlesBackgroundOpacityValue() const;
	UFUNCTION(BlueprintCallable)
	float GetSubtitlesSizeValue() const;
	UFUNCTION(BlueprintCallable)
	bool GetHardOfEaringMod() const;
};
