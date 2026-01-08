// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Dialogue/AnswerButtonWidget.h"

#include "CommonTextBlock.h"
#include "Components/RichTextBlock.h"

void UAnswerButtonWidget::SetText(const FText& NewText)
{
	Text->SetText(NewText);
}

void UAnswerButtonWidget::NativeOnHovered()
{
	Text->SetStyle(HoveredTextStyle);
	Super::NativeOnHovered();
}

void UAnswerButtonWidget::NativeOnUnhovered()
{
	Text->SetStyle(NormalTextStyle);
	Super::NativeOnUnhovered();
}
