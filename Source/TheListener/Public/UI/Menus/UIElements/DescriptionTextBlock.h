// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "DescriptionTextBlock.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API UDescriptionTextBlock : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	void UpdateDescription(const FText& Description, float Y) const;

protected:
	UPROPERTY(meta=(BindWidget))
	class UCommonTextBlock* DescriptionTextBlock;

	UPROPERTY(meta=(BindWidget))
	class USizeBox* DescriptionBox;
};
