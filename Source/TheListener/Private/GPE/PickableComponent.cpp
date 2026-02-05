// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "GPE/PickableComponent.h"

#include "Miscellaneous/TLUtils.h"

UPickableComponent::UPickableComponent()
{
}

bool UPickableComponent::Allows(AActor const * Actor)
{
	return Allowed.Contains(Actor);
}

void UPickableComponent::ShowPrompts() const
{
	UTLUtils::TogglePrompts(GetWorld(), ActivePrompts, true);
}

void UPickableComponent::HidePrompts() const
{
	UTLUtils::TogglePrompts(GetWorld(), ActivePrompts, false);
}

