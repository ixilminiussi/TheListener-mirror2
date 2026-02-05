// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "UI/Prompt/HoverPromptComponent.h"

#include "Miscellaneous/TLUtils.h"

UHoverPromptComponent::UHoverPromptComponent()
{
}

void UHoverPromptComponent::TogglePrompts(bool const bToggle) const
{
	UTLUtils::TogglePrompts(GetWorld(), PromptNames, bToggle);
}

