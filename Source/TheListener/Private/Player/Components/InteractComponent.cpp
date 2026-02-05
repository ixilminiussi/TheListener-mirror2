// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "Player/Components/InteractComponent.h"

#include "GPE/Interactable.h"
#include "UI/LukaHUD.h"

// Sets default values for this component's properties
UInteractComponent::UInteractComponent()
{
}


// Called when the game starts
void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();
}

AActor *UInteractComponent::CheckHitResults(TArray<FHitResult>* HitResults, AActor *Key)
{
	int i = 0;
	for (; i < HitResults->Num(); i++)
	{
		if (AInteractable *Interactable = Cast<AInteractable>((*HitResults)[i].GetActor()))
		{
			if (Interactable->TestKey(Key) && Interactable->IsEnabled())
			{
				return Interactable;
			}
		}
	}

	return nullptr;
}

bool UInteractComponent::Interact(AActor* Actor, AActor *Key)
{
	if (AInteractable *Interactable = Cast<AInteractable>(Actor))
	{
		return Interactable->Interact(Key);
	}

	return false;
}

