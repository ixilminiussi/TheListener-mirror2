// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "SwitcherTabSettings.generated.h"

UCLASS()
class THELISTENER_API USwitcherTabSettings : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	class UCommonAnimatedSwitcher* GetAnimatedSwitcher() const;

	class UHorizontalBox* GetTabHeaderBox() const;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION()
	void SetIndex(int Index, float WaitingTime) const;

	UFUNCTION()
	void SetFocusOnElement(float WaitingTime) const;

protected:
	UPROPERTY(meta = (BindWidget))
	class USettingsTabBase* GameplayTab;
	UPROPERTY(meta = (BindWidget))
	class USettingsTabBase* AudioTab;
	UPROPERTY(meta = (BindWidget))
	class USettingsTabBase* GraphicsTab;
	UPROPERTY(meta = (BindWidget))
	class USettingsTabBase* AccessibilityTab;

	//Tab Header
	UPROPERTY(meta = (BindWidget))
	class UWidgetTab* GameplayTabHeader;
	UPROPERTY(meta = (BindWidget))
	class UWidgetTab* AudioTabHeader;
	UPROPERTY(meta = (BindWidget))
	class UWidgetTab* GraphicsTabHeader;
	UPROPERTY(meta = (BindWidget))
	class UWidgetTab* AccessibilityTabHeader;
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* TabHeaderBox;


	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonActivatableWidgetSwitcher* AnimatedSwitcher;

	UFUNCTION()
	void ToRightTab() const;
	UFUNCTION()
	void ToLeftTab() const;

public:
	UFUNCTION()
	USettingsTabBase* GetGameplayTab() const;
	UFUNCTION()
	USettingsTabBase* GetAudioTab() const;
	UFUNCTION()
	USettingsTabBase* GetGraphicsTab() const;
	UFUNCTION()
	USettingsTabBase* GetAccessibilityTab() const;

	UFUNCTION()
	UWidgetTab* GetGameplayTabHeader() const;
	UFUNCTION()
	UWidgetTab* GetAudioTabHeader() const;
	UFUNCTION()
	UWidgetTab* GetGraphicsTabHeader() const;
	UFUNCTION()
	UWidgetTab* GetAccessibilityTabHeader() const;
};
