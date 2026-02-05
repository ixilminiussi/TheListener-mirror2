// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THELISTENER_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractComponent();
	
	AActor *CheckHitResults(TArray<FHitResult>* HitResults, AActor *Key);
	[[nodiscard]] bool Interact(AActor *Actor, AActor *Key = nullptr);
	
protected:
	virtual void BeginPlay() override;
};
