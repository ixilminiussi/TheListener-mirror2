// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "UI/SelectorBox.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void USelectorBox::SaveCurrentAlpha()
{
	FLinearColor Color = SelectorBrush.GetTint(FWidgetStyle());
	Alpha = Color.A;
}

void USelectorBox::SetSlotInformation(FVector2D NewPos, FVector2D NewSize)
{
	Position = NewPos;
	PositionMax = Position + NewSize;
}

bool USelectorBox::IsSelectorInside(FVector2D SelectorPos)
{
	bool IsInside =(
	SelectorPos.X <= PositionMax.X &&
	SelectorPos.X >= Position.X &&
	SelectorPos.Y <= PositionMax.Y &&
	SelectorPos.Y >= Position.Y 
	);
	SetAlpha(IsInside ? Alpha : 0);
	return IsInside;
}

void USelectorBox::SelectorHover()
{
	SetAlpha(Alpha);
}

void USelectorBox::Discover()
{
	bIsDiscovered = true;
}

void USelectorBox::SetAlpha(float NewAlpha)
{
	FLinearColor Color = SelectorBrush.GetTint(FWidgetStyle());
	Color.A = NewAlpha;
	SelectorBrush.TintColor = FSlateColor(Color);
}

FString USelectorBox::GetSelectorName()
{
	return SelectorName;
}

void USelectorBox::NativeConstruct()
{
	SaveCurrentAlpha();
	SetAlpha(0);
}
