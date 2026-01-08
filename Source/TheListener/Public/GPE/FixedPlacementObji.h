// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GPE/Obji.h"
#include "FixedPlacementObji.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class THELISTENER_API AFixedPlacementObji : public AObji
{
	GENERATED_BODY()

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void RotatePreview(const struct FInputActionValue& Value) override;

	UFUNCTION(BlueprintCallable, Category = "FixedPlacementObji")
	void AddDropLocation(FTransform NewDropLocation, float Radius);

protected:
	virtual void CustomCheck(FDropZoneInfo& DropZoneInfo, const FVector& RayCheckBegin,
	                         const FVector& RayCheckEnd, FCollisionQueryParams QueryParams = {}) override;

	virtual void OnInteract(class AController* NewController) override;
	
	virtual void OnDrop() override;

	TArray<TObjectPtr<AActor>> DropLocations;
};
