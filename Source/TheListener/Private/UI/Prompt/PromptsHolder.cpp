// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#include "UI/Prompt/PromptsHolder.h"

#include "Blueprint/WidgetTree.h"

void UPromptsHolder::NativeConstruct()
{
	Super::NativeConstruct();

	TArray<UWidget*> Widgets;
	WidgetTree->GetAllWidgets(Widgets);

	for (UWidget* Widget : Widgets)
	{
		if (Widget->GetName().StartsWith("PRT_"))
		{
			Widget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UPromptsHolder::Show(FName const& Name) const
{
	UWidget *Widget = GetWidgetFromName(Name);

	if (ensure(Widget)) // Most likely you forgot to write the name of a prompt in parameters
	{
		Widget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPromptsHolder::Hide(FName const& Name) const
{
	UWidget *Widget = GetWidgetFromName(Name);

	if (ensure(Widget)) // Most likely you forgot to write the name of a prompt in parameters
	{
		Widget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

