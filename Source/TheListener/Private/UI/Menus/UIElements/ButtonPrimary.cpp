// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/UIElements/ButtonPrimary.h"

#include "CommonTextBlock.h"
#include "Components/TextBlock.h"

void UButtonPrimary::NativeConstruct()
{
	Super::NativeConstruct();

	if (!TextBlock)
	{
		return;
	}

	TextBlock->SetText(Text);
	TextBlock->SetStyle(NormalTextStyle);
}

void UButtonPrimary::NativeOnHovered()
{
	check(TextBlock);
	TextBlock->SetStyle(HoveredTextStyle);

	Super::NativeOnHovered();
}

void UButtonPrimary::NativeOnUnhovered()
{
	check(TextBlock);
	TextBlock->SetStyle(NormalTextStyle);

	Super::NativeOnUnhovered();
}

void UButtonPrimary::NativeOnPressed()
{
	check(TextBlock);
	TextBlock->SetStyle(PressedTextStyle);

	Super::NativeOnPressed();
}
