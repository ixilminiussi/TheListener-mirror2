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
}

void UButtonUIElement::NativeOnHovered()
{
	Super::NativeOnHovered();
}

void UButtonUIElement::UpdateDescriptionText() const
{
	TArray<UUserWidget*> Widgets{};
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), Widgets, UDescriptionTextBlock::StaticClass(), false);

	if (Widgets.Num() > 0)
	{
		if (UDescriptionTextBlock* DescriptionTextBlock = Cast<UDescriptionTextBlock>(Widgets[0]))
		{
			if (FSettingsDescriptions* SettingsDescription = DescriptionRow.GetRow<FSettingsDescriptions>(
				TEXT("Loading Settings Description")); SettingsDescription)
			{
				check(DescriptionTextBlock);
				const FText Description = SettingsDescription->EnglishDescription;

				const FVector2D Absolute = GetCachedGeometry().GetAbsolutePosition();
				const FVector2D Relative = GetParent()->GetCachedGeometry().AbsoluteToLocal(Absolute);

				const FVector2D Size = GetCachedGeometry().GetAbsoluteSize();


				DescriptionTextBlock->UpdateDescription(Description, Relative.Y);
			}
		}
	}
}
