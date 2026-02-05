// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonLazyImage.h"
#include "PromptImage.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API UPromptImage : public UCommonLazyImage
{
	GENERATED_BODY()

public:
	UFUNCTION(BLueprintCallable)
	void Refresh(TArray<struct FEnhancedActionKeyMapping> const &Mapping);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	TObjectPtr<class UInputAction> InputAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	int Index = 0;
};
