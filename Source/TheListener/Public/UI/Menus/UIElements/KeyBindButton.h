// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "Components/InputKeySelector.h"
#include "UI/Menus/UIElements/ButtonUIElement.h"
#include "CommonBorder.h"
#include "KeyBindButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRebindCaptured, UKeyBindButton*, Button, FKey, NewKey);

UCLASS()
class THELISTENER_API UKeyBindButton : public UButtonUIElement
{
	GENERATED_BODY()

public:
	void Init(FName KeyName, const FText& KeyDisplayName, FEnhancedActionKeyMapping& Key);

	UPROPERTY(BlueprintAssignable)
	FOnRebindCaptured OnRebindCaptured;

	UFUNCTION(Category = "Input Rebinding")
	void StartListening();

	UFUNCTION()
	void UpdateVisual(FKey& Key);

	UFUNCTION()
	class UPromptImage* GetPromptImage();

	FKey GetBoundKey() const;
	UPROPERTY()
	FLinearColor CurrentColor;

protected:
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual FReply
	NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent) override;

	virtual void NativeOnClicked() override;

	virtual void NativeOnHovered() override;

	virtual void NativeOnUnhovered() override;


	UPROPERTY(meta=(BindWidget))
	class UCommonTextBlock* InputTextBlock;
	FName InputName;

	UPROPERTY()
	FEnhancedActionKeyMapping DisplayedKey;

	UPROPERTY(meta=(BindWidget))
	class UPromptImage* PromptImage;
	UPROPERTY(meta=(BindWidget))
	UCommonBorder* CommonBorder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UIElement|KeyBindStyle|Text")
	TSubclassOf<UCommonTextStyle> NormalTextStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UIElement|KeyBindStyle|Text")
	TSubclassOf<UCommonTextStyle> HoveredTextStyle;

private:
	bool bIsListening = false;

	UPROPERTY()
	FKey BoundKey;
};
