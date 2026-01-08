// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "TimedPromptWidget.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API UTimedPromptWidget : public UCommonUserWidget
{
	GENERATED_BODY()

	public:
	void SetPercentage(float Percentage);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> ProgressBar;
};
