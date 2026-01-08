// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "AnswerButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API UAnswerButtonWidget : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,meta=(BindWidget),Category = "System|Dialogue|Choice Buttons")
	class UCommonTextBlock* Text;

	void SetText(const FText& NewText);

	virtual void NativeOnHovered() override;

	virtual void NativeOnUnhovered() override;
	
	protected:
	UPROPERTY(EditAnywhere, Category="System|Dialogue|TextStyle")
	TSubclassOf<UCommonTextStyle> NormalTextStyle;
	UPROPERTY(EditAnywhere, Category="System|Dialogue|TextStyle")
	TSubclassOf<UCommonTextStyle> HoveredTextStyle;
	
};
