// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "DescriptionTextBlock.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API UDescriptionTextBlock : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	class UCommonTextBlock* GetTextBlock();

protected:
	UPROPERTY(meta=(BindWidget))
	class UCommonTextBlock* DescriptionTextBlock;
};
