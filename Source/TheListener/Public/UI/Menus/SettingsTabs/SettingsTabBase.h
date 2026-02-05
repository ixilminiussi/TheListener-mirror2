// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonUserWidget.h"
#include "SettingsTabBase.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API USettingsTabBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void SetupSettingsValues(class USettingsSave* SettingSave);

	class UScrollBox* GetScrollBox() const;

	virtual void NativeOnActivated() override;

	UPROPERTY(EditAnywhere)
	bool bIsKeyBind = false;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UScrollBox* ScrollBox;
};
