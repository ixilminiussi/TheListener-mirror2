// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
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
	bool ControllerHapticFeedback;

	//Graphics
	UPROPERTY(EditDefaultsOnly, Category = "Graphics")
	int32 ResolutionDropdownIndex;
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

	//Graphics
	UFUNCTION()
	void SetResolutionDropdownIndex(int32 Index);
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


	// Getters
	// Gameplay
	UFUNCTION()
	float GetHorizontalSensitivity() const;
	UFUNCTION()
	float GetVerticalSensitivity() const;
	UFUNCTION()
	bool GetControllerHapticFeedback() const;

	//Graphics
	UFUNCTION()
	int32 GetResolutionDropdownIndex() const;
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
};
