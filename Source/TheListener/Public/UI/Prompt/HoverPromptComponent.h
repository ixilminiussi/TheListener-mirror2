// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HoverPromptComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THELISTENER_API UHoverPromptComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHoverPromptComponent();

	void TogglePrompts(bool const bToggle) const;

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<FName> PromptNames;
};
