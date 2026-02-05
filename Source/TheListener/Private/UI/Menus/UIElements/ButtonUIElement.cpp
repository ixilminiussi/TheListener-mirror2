// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/UIElements/ButtonUIElement.h"

#include "CommonTextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "Transform/TransformConstraintUtil.h"
#include "UI/Menus/UIElements/DescriptionTextBlock.h"

void UButtonUIElement::NativeConstruct()
{
	Super::NativeConstruct();

	SetNavigationRuleExplicit(EUINavigation::Up, UpUIElement);
	SetNavigationRuleExplicit(EUINavigation::Down, DownUIElement);

	if (DescriptionRow.IsNull())
	{
		DescriptionTextBlock->RemoveFromParent();
		return;
	}

	UpdateDescriptionText();

	if (DescriptionTextBlock)
	{
		DescriptionTextBlock->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UButtonUIElement::NativeOnHovered()
{
	Super::NativeOnHovered();
	if (DescriptionTextBlock)
	{
		DescriptionTextBlock->SetVisibility(ESlateVisibility::Visible);
		UpdateDescriptionText();
	}
}

void UButtonUIElement::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();

	if (DescriptionTextBlock)
	{
		DescriptionTextBlock->SetVisibility(ESlateVisibility::Hidden);
		DescriptionTextBlock->GetTextBlock()->SetText(FText::GetEmpty());
	}
}

void UButtonUIElement::UpdateDescriptionText() const
{
	if (FSettingsDescriptions* SettingsDescription = DescriptionRow.GetRow<FSettingsDescriptions>(
		TEXT("Loading Settings Description")))
	{
		check(DescriptionTextBlock);
		const FText Description = SettingsDescription->EnglishDescription;
		DescriptionTextBlock->GetTextBlock()->SetText(Description);
	}
}
