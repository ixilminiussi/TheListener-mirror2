// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "UI/Menus/SettingsTabs/SettingsTabBase.h"
#include "SettingsKeyBindWidget.generated.h"

/**
 * 
 */

UCLASS()
class THELISTENER_API USettingsKeyBindWidget : public USettingsTabBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeOnActivated() override;

	void DisplayKeys();

protected:
	UPROPERTY(EditAnywhere, Category = "Settings|Input's Maps")
	TArray<UInputMappingContext*> AllMaps;

	UPROPERTY(EditAnywhere, Category = "Settings")
	TSubclassOf<class UUserWidget> KeyBindButtonClass;

	UPROPERTY()
	TMap<class UKeyBindButton*, UInputMappingContext*> ButtonToContext;

	UPROPERTY()
	TMap<UKeyBindButton*, FEnhancedActionKeyMapping> ButtonToMapping;


	UPROPERTY()
	TArray<class UKeyBindButton*> KeyBindsButtons;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	class TSubclassOf<class UCommonTextStyle> TextStyle;

	UFUNCTION()
	void SetupNavigation();

	UFUNCTION()
	void HandleKeyRebound(UKeyBindButton* Button, FKey NewKey);
};
