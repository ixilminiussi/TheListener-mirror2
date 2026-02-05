// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickupComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THELISTENER_API UPickupComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UPickupComponent();

	AActor *CheckAndFilterHitResults(TArray<FHitResult> *HitResults);
	UFUNCTION(BlueprintCallable)
	void TryPickup(AActor *Actor);
	void TryDrop();
	void Consume();
	void Pass(UPickupComponent *);

	AActor *GetHeld();
	
protected:
	UPROPERTY()
	TObjectPtr<AActor> HeldActor;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	float ThrowSpeed; 
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	TArray<FName> GeneralPrompts;
	
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
};

inline AActor* UPickupComponent::GetHeld()
{
	return HeldActor;
}
