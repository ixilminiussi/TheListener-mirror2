// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "PromptWidget.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API UPromptWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	TArray<TObjectPtr<class UPromptImage>> const &GetPromptsImages() const;

protected:
	TArray<TObjectPtr<class UPromptImage>> PromptsImages;
};
