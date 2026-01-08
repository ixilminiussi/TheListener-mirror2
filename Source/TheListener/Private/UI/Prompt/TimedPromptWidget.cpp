// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.
#include "UI/Prompt/TimedPromptWidget.h"

#include "Components/ProgressBar.h"

void UTimedPromptWidget::SetPercentage(float Percentage)
{
	if (ProgressBar)
	{
		ProgressBar->SetPercent(Percentage);
	}
}
