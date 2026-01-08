// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "PromptsHolder.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API UPromptsHolder : public UCommonUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	void Show(FName const &Name) const;
	void Hide(FName const &Name) const;
};
