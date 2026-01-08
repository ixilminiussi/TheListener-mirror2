// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "UI/Menus/UIElements/DescriptionTextBlock.h"

#include "CommonTextBlock.h"
#include "Components/SizeBox.h"

void UDescriptionTextBlock::UpdateDescription(const FText& Description, const float Y) const
{
	DescriptionTextBlock->SetText(Description);

	const float X = DescriptionBox->GetRenderTransform().Translation.X;

	DescriptionBox->SetRenderTranslation(FVector2D(X, Y));
}
