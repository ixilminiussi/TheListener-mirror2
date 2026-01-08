// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SelectorBox.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class THELISTENER_API USelectorBox : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Files|UI", meta = (BindWidget))
	class UImage* SelectorBoxImage;

	UPROPERTY(EditAnywhere, Category = "Toy|Files", meta = (BindWidget))
	FString SelectorName;

	FVector2D Position;
	FVector2D PositionMax;
	float Alpha = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDiscovered = false;

public:
	UPROPERTY(EditAnywhere, Category = "Toy|Files|Appearance", BlueprintReadWrite)
	FSlateBrush SelectorBrush;
	UPROPERTY(EditAnywhere, Category = "Toy|Files|Appearance", BlueprintReadWrite)
	FSlateBrush SelectorDiscoveredBrush;

	void SaveCurrentAlpha();

	void SetSlotInformation(FVector2D NewPos, FVector2D NewSize);
	bool IsSelectorInside(FVector2D SelectorPos);

	void SelectorHover();
	void Discover();

	void SetAlpha(float NewAlpha);

	FString GetSelectorName();

	virtual void NativeConstruct() override;
};
