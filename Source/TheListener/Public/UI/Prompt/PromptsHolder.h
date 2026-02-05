// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "EnhancedActionKeyMapping.h"
#include "PromptsHolder.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FShowPrompt, FName, TArray<FEnhancedActionKeyMapping>);
DECLARE_MULTICAST_DELEGATE_OneParam(FHidePrompt, FName);

/**
 * 
 */
UCLASS()
class THELISTENER_API UPromptsHolder : public UCommonUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
public:
	static FShowPrompt ShowPrompt;
	static FHidePrompt HidePrompt;

protected:
	UFUNCTION()
	void Show(FName const& Name, TArray<struct FEnhancedActionKeyMapping> const &Mapping) const;
	UFUNCTION()
	void Hide(FName const &Name) const;
};
