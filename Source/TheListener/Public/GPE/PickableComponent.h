// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickup);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THELISTENER_API UPickableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPickableComponent();

	bool Allows(AActor const *);

	UPROPERTY(BlueprintAssignable, Category = "Pickable")
	FOnPickup OnPickup;
	
	void ShowPrompts() const;
	void HidePrompts() const;
protected:
	UPROPERTY(EditAnywhere, Category = "Pickable")
	TArray<TObjectPtr<class AActor>> Allowed;

	UPROPERTY(EditDefaultsOnly, Category = "Pickable")
	TArray<FName> ActivePrompts;
};
