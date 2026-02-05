// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#include "UI/Prompt/PromptsHolder.h"

#include "Blueprint/WidgetTree.h"
#include "UI/Prompt/PromptImage.h"
#include "UI/Prompt/PromptWidget.h"

FShowPrompt UPromptsHolder::ShowPrompt;
FHidePrompt UPromptsHolder::HidePrompt;

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

	ShowPrompt.AddUFunction(this, FName("Show"));
	HidePrompt.AddUFunction(this, FName("Hide"));
}

void UPromptsHolder::NativeDestruct()
{
	Super::NativeDestruct();

	ShowPrompt.RemoveAll(this);
	HidePrompt.RemoveAll(this);
}

void UPromptsHolder::Show(FName const& Name, TArray<FEnhancedActionKeyMapping> const &Mapping) const
{
	UWidget *Widget = GetWidgetFromName(Name);
	if (!ensure(Widget)) 
	{
		// WARN: YOU FORGOT TO WRITE THE NAME OF A PROMPT IN PARAMETERS OR ITS THE WRONG NAME
		return;
	}
	
	Widget->SetVisibility(ESlateVisibility::Visible);
	
	UPromptWidget *Prompt = Cast<UPromptWidget>(Widget);
	if (!ensure(Prompt)) 
	{
		// WARN: YOU ARE TOGGLING A PROMPT WHICH ISNT OF TYPE PROMPT WIDGET
		return;
	}

	for (UPromptImage *PromptImage : Prompt->GetPromptsImages())
	{
		PromptImage->Refresh(Mapping);
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

