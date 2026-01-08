// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "WidgetTab.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API UWidgetTab : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativePreConstruct() override;

	// Get/SetIndex()
	void SetIndex(int NewIndex);

	int GetIndex() const;

	void Toggle(bool bToggle);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class URichTextBlock* RichTextBlock;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UImage* LineImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	FText Text;

	UPROPERTY()
	int Index;
};
