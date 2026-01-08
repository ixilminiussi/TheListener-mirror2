// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CommonTextBlocDropDown.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API UCommonTextBlocDropDown : public UCommonUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	class UCommonTextBlock* GetCommonTextBlock() const;

protected:
	UPROPERTY(meta=(BindWidget))
	class UCommonTextBlock* CommonTextBlock;
};
