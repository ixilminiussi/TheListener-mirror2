// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"


UCLASS()
class THELISTENER_API ABaseHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	virtual class UBaseMenuWidget* GetPreviousWidget();

	UFUNCTION()
	virtual class USettingsMenuWidget* GetSettingsMenuWidget() const;

protected:
	UPROPERTY(BlueprintReadWrite)
	UBaseMenuWidget* PreviousWidgetInstance = nullptr;

	UPROPERTY(BlueprintReadWrite)
	USettingsMenuWidget* SettingsMenuWidgetInstance = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget|Menus")
	TSubclassOf<class UBaseMenuWidget> PreviousWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget|Menus")
	TSubclassOf<class USettingsMenuWidget> SettingsMenuWidgetClass;

	UPROPERTY()
	bool bIsPaused{false};


public:
	UFUNCTION(BlueprintImplementableEvent)
	void OpenSettingsInBlueprint();
	UFUNCTION(BlueprintImplementableEvent)
	void CloseSettingsInBlueprint();
};
