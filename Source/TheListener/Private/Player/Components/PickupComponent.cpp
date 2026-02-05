// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "Player/Components/PickupComponent.h"

#include "GPE/Interactable.h"
#include "GPE/PickableComponent.h"
#include "GPE/Toy.h"
#include "Miscellaneous/TLUtils.h"
#include "Player/LukaCharacter.h"
#include "UI/LukaHUD.h"

UPickupComponent::UPickupComponent()
{
}

void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPickupComponent::BeginDestroy()
{
	Super::BeginDestroy();
}

AActor *UPickupComponent::CheckAndFilterHitResults(TArray<FHitResult>* HitResults) 
{
	if (HeldActor)
	{
		UPickableComponent *PickableComponent = Cast<UPickableComponent>(HeldActor->GetComponentByClass(UPickableComponent::StaticClass()));

		HitResults->RemoveAll([this, PickableComponent](FHitResult &Result)
		{
			if (AToy *Toy = Cast<AToy>(Result.GetActor()); Toy)
			{
				return !(Toy->GetComponentByClass(UPickupComponent::StaticClass()));
			};
			if (AInteractable *Interactable = Cast<AInteractable>(Result.GetActor()); Interactable)
			{
				return !(PickableComponent->Allows(Interactable) || Interactable->TestKey(GetOwner()));
			};
			return !(PickableComponent->Allows(Result.GetActor()));
		});
		return nullptr;
	}
	for (int i = 0; i < HitResults->Num(); i++)
	{
		if (UActorComponent *PickableComponent = (*HitResults)[i].GetActor()->GetComponentByClass(UPickableComponent::StaticClass()); PickableComponent != nullptr)
		{
			return PickableComponent->GetOwner();
		}
	}

	return nullptr;
}

void UPickupComponent::TryPickup(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}
	UPickableComponent *PickableComponent = Actor->GetComponentByClass<UPickableComponent>();
	if (!PickableComponent)
	{
		return;
	}
	if (HeldActor)
	{
		return;
	}

	for (UActorComponent* Component : Actor->K2_GetComponentsByClass(UStaticMeshComponent::StaticClass()))
	{
		UStaticMeshComponent *StaticMesh = Cast<UStaticMeshComponent>(Component);
		StaticMesh->SetSimulatePhysics(false);
		StaticMesh->SetEnableGravity(false);
	}
	
	FAttachmentTransformRules TransformRules{EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true};
Actor->AttachToComponent(this, TransformRules);

	HeldActor = Actor;
	PickableComponent->ShowPrompts();
	PickableComponent->OnPickup.Broadcast();
	UTLUtils::TogglePrompts(GetWorld(), GeneralPrompts, true);
}

void UPickupComponent::TryDrop()
{
	if (!HeldActor)
	{
		return;
	}
	
	FDetachmentTransformRules TransformRules{EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false};
	HeldActor->DetachFromActor(TransformRules);

	if (const ALukaCharacter *Luka = Cast<ALukaCharacter>(GetOwner()))
	{
		USceneComponent* DropComponent = Luka->GetDrop();
		HeldActor->SetActorLocationAndRotation(DropComponent->GetComponentLocation(), DropComponent->GetComponentRotation(), false, nullptr, ETeleportType::ResetPhysics);
	}

	UStaticMeshComponent *StaticMesh = Cast<UStaticMeshComponent>(HeldActor->GetRootComponent());
	if (ensure(StaticMesh)) // TODO: THE ROOT NODE OF THAT ACTOR NEEDS TO BE A STATIC MESH YA IDIOT
	{
		StaticMesh->SetSimulatePhysics(true);
		StaticMesh->SetEnableGravity(true);

		FVector Impulse = GetOwner()->GetActorForwardVector() * ThrowSpeed;
		StaticMesh->AddImpulse(Impulse);
	}

	UPickableComponent *PickableComponent = HeldActor->GetComponentByClass<UPickableComponent>();
	check(PickableComponent);
	PickableComponent->HidePrompts();
	UTLUtils::TogglePrompts(GetWorld(), GeneralPrompts, false);
	
	HeldActor = nullptr;
}

void UPickupComponent::Consume() 
{
	UTLUtils::TogglePrompts(GetWorld(), GeneralPrompts, false);
	HeldActor->Destroy();
	HeldActor = nullptr;
}

void UPickupComponent::Pass(UPickupComponent* PickupComponent)
{
	check(PickupComponent);
	
	if (!HeldActor)
	{
		return;
	}
	
	FDetachmentTransformRules TransformRules{EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false};
	HeldActor->DetachFromActor(TransformRules);

	UTLUtils::TogglePrompts(GetWorld(), GeneralPrompts, false);

	PickupComponent->TryPickup(HeldActor);

	HeldActor = nullptr; 
}

