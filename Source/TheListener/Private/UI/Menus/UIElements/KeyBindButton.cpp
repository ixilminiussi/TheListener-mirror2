// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "UI/Menus/UIElements/KeyBindButton.h"

#include "CommonTextBlock.h"
#include "EnhancedActionKeyMapping.h"
#include "Components/InputKeySelector.h"
#include "Kismet/GameplayStatics.h"
#include "System/Core/BaseGameInstance.h"
#include "UI/Menus/UIElements/InputKeySelectorSettings.h"
#include "UI/Prompt/PromptImage.h"

void UKeyBindButton::Init(const FName KeyName, const FText& KeyDisplayName, FEnhancedActionKeyMapping& Key)
{
	InputName = KeyName;
	DisplayedKey = Key;

	InputTextBlock->SetText(KeyDisplayName);
	InputTextBlock->SetStyle(NormalTextStyle);

	BoundKey = Key.Key;

	UpdateVisual(Key.Key);
}

class UPromptImage* UKeyBindButton::GetPromptImage()
{
	return PromptImage;
}

void UKeyBindButton::StartListening()
{
	PromptImage->SetColorAndOpacity(FLinearColor::Yellow);
	bIsListening = true;
	SetKeyboardFocus();
}

void UKeyBindButton::UpdateVisual(FKey& Key)
{
	UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	check(GameInstance);

	const int ID = GameInstance->FindAssociatedInputMappingId(Key);

	FSlateBrush NewBrush;
	NewBrush.SetResourceObject(GameInstance->GetCurrentControllerTypeTexture(ID));
	NewBrush.ImageSize = FVector2D(48.f, 48.f);

	PromptImage->SetBrush(NewBrush);

	BoundKey = Key;
}

FKey UKeyBindButton::GetBoundKey() const
{
	return BoundKey;
}

FReply UKeyBindButton::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!bIsListening)
	{
		return FReply::Unhandled();
	}

	FKey PressedKey = InKeyEvent.GetKey();

	if (InKeyEvent.IsRepeat())
	{
		return FReply::Handled();
	}

	static const TSet<FKey> BlackListedKeys =
	{
		EKeys::Gamepad_Special_Left,
	};

	if (BlackListedKeys.Contains(PressedKey))
	{
		return FReply::Handled();
	}

	if (PressedKey == EKeys::Gamepad_Special_Right)
	{
		bIsListening = false;
		PromptImage->SetColorAndOpacity(CurrentColor);
		return FReply::Handled();
	}

	if (PressedKey.IsGamepadKey())
	{
		bIsListening = false;
		OnRebindCaptured.Broadcast(this, PressedKey);
		UpdateVisual(PressedKey);
		return FReply::Handled();
	}

	return FReply::Handled();
}

FReply UKeyBindButton::NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent)
{
	if (!bIsListening)
	{
		return FReply::Unhandled();
	}

	FKey AnalogKey = InAnalogEvent.GetKey();

	if (InAnalogEvent.IsRepeat())
	{
		return FReply::Handled();
	}
	static const TSet<FKey> BlackListedKeys =
	{
		EKeys::Gamepad_LeftX,
		EKeys::Gamepad_LeftY,
		EKeys::Gamepad_RightX,
		EKeys::Gamepad_RightY,
		EKeys::Gamepad_LeftTriggerAxis,
		EKeys::Gamepad_RightTriggerAxis
	};

	if (BlackListedKeys.Contains(AnalogKey))
	{
		return FReply::Handled();
	}

	if (AnalogKey.IsGamepadKey())
	{
		bIsListening = false;
		OnRebindCaptured.Broadcast(this, AnalogKey);
		UpdateVisual(AnalogKey);
		return FReply::Handled();
	}

	return FReply::Handled();
}

void UKeyBindButton::NativeOnClicked()
{
	Super::NativeOnClicked();

	CurrentColor = PromptImage->GetColorAndOpacity();

	bIsListening = true;
	StartListening();
}


void UKeyBindButton::NativeOnHovered()
{
	InputTextBlock->SetStyle(HoveredTextStyle);
	//InputText->SetStyle(HoveredTextStyle);

	Super::NativeOnHovered();
}

void UKeyBindButton::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();

	InputTextBlock->SetStyle(NormalTextStyle);

	if (bIsListening)
	{
		PromptImage->SetColorAndOpacity(CurrentColor);
	}
}
