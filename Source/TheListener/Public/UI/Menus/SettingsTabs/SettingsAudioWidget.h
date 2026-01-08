// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SettingsTabBase.h"
#include "SettingsAudioWidget.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API USettingsAudioWidget : public USettingsTabBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class USliderSettings* MasterSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class USliderSettings* RadioStaticSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class USliderSettings* MusicSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class USliderSettings* RadioSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class USliderSettings* SFXSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class USliderSettings* LukaSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class USliderSettings* AmbientSlider;

	UFUNCTION()
	void OnMasterSliderValueChanged(float Value);
	UFUNCTION()
	void OnRadioStaticSliderValueChanged(float Value);
	UFUNCTION()
	void OnMusicSliderValueChanged(float Value);
	UFUNCTION()
	void OnRadioSliderValueChanged(float Value);
	UFUNCTION()
	void OnSFXSliderValueChanged(float Value);
	UFUNCTION()
	void OnLukaSliderValueChanged(float Value);
	UFUNCTION()
	void OnAmbientSliderValueChanged(float Value);
};
