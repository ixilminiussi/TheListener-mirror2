// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/UIElements/WidgetTab.h"

#include "Components/Image.h"
#include "Components/RichTextBlock.h"

void UWidgetTab::NativeConstruct()
{
	Super::NativeConstruct();

	if (!RichTextBlock)
	{
		return;
	}
	RichTextBlock->SetText(Text);
}

void UWidgetTab::NativePreConstruct()
{
	Super::NativeConstruct();

	if (!RichTextBlock)
	{
		return;
	}

	RichTextBlock->SetText(Text);
}


void UWidgetTab::SetIndex(int NewIndex)
{
	Index = NewIndex;
}

int UWidgetTab::GetIndex() const
{
	return Index;
}

void UWidgetTab::Toggle(bool bToggle)
{
	if (bToggle)
	{
		check(LineImage);
		LineImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		check(LineImage);
		LineImage->SetVisibility(ESlateVisibility::Hidden);
	}
}
