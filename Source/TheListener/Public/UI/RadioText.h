// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadioText.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API URadioText : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRadioText(const FText& Text) const;

protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextBlock;
};
