// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "ButtonPrimary.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API UButtonPrimary : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeOnHovered() override;

	virtual void NativeOnUnhovered() override;

	virtual void NativeOnPressed() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UCommonTextBlock* TextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button|TextStyle")
	TSubclassOf<UCommonTextStyle> NormalTextStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button|TextStyle")
	TSubclassOf<UCommonTextStyle> HoveredTextStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button|TextStyle")
	TSubclassOf<UCommonTextStyle> PressedTextStyle;
};
