// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "UI/RadioText.h"

#include "Components/TextBlock.h"

void URadioText::SetRadioText(const FText& Text) const
{
	TextBlock->SetText(Text);
}
