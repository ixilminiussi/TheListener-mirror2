// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "ButtonUIElement.generated.h"

UENUM(BlueprintType)
enum ESettingsDescriptionCategory
{
	AudioTab,
	GraphicTab,
	GameplayTab,
	AccessibilityTab,
};

USTRUCT(BlueprintType)
struct FSettingsDescriptions : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ESettingsDescriptionCategory> SettingsType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText EnglishDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText FrenchDescription;
};

UCLASS()
class THELISTENER_API UButtonUIElement : public UCommonButtonBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnHovered() override;

	void UpdateDescriptionText() const;


	UPROPERTY()
	bool bIsEditing = false;

	UPROPERTY(EditAnywhere, Category="UIElement|Navigation")
	UButtonUIElement* UpUIElement;

	UPROPERTY(EditAnywhere, Category="UIElement|Navigation")
	UButtonUIElement* DownUIElement;

	UPROPERTY(EditAnywhere, Category="UIElement|Description")
	FDataTableRowHandle DescriptionRow;
};
