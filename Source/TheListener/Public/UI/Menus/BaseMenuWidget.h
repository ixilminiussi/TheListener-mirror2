// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BaseMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API UBaseMenuWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	virtual class UButtonPrimary* GetFocusedButton() const;

	virtual void NativeConstruct() override;

	UFUNCTION()
	virtual class UButton* GetPreMenuButton();

protected:

	// Buttons Functions
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void OnChangeLevelClicked();

	UFUNCTION()
	virtual void OnSettingsClicked() const;

	UFUNCTION()
	virtual void OnQuitClicked() const;

	UFUNCTION()
	virtual void OnYesQuitClicked() const;

	UFUNCTION()
	virtual void OnNoQuitClicked() const;

	
	// Widgets Button
	UPROPERTY(EditAnywhere ,meta = (BindWidget))
	UButtonPrimary* ChangeLevelButton;

	UPROPERTY(meta = (BindWidget))
	UButtonPrimary* SettingsButton;
	
	UPROPERTY(meta = (BindWidget))
	UButtonPrimary* QuitButton;

	UPROPERTY(BlueprintReadWrite ,meta = (BindWidget))
	class UCanvasPanel* MainPanel;

	
	// Variables for GD
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Menus", meta = (AllowedClasses="World"))
	TSoftObjectPtr<UWorld> NewLevel;

	// Quit Button
	UPROPERTY(EditAnywhere ,meta = (BindWidget))
	UButtonPrimary* YesQuitButton;

	UPROPERTY(meta = (BindWidget))
	UButtonPrimary* NoQuitButton;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* QuitPanel;

};
