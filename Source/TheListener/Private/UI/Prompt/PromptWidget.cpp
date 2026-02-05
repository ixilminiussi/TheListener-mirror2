// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "UI/Prompt/PromptWidget.h"

#include "Blueprint/WidgetTree.h"
#include "UI/Prompt/PromptImage.h"

void UPromptWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TArray<UWidget*> ChildWidgets;
	WidgetTree->GetChildWidgets(WidgetTree->RootWidget, ChildWidgets);

	for (UWidget* ChildWidget : ChildWidgets)
	{
		if (ChildWidget->IsA(UPromptImage::StaticClass()))
		{
			PromptsImages.Add(Cast<UPromptImage>(ChildWidget));
		}
	}
}

TArray<TObjectPtr<UPromptImage>> const& UPromptWidget::GetPromptsImages() const
{
	return PromptsImages;
}
