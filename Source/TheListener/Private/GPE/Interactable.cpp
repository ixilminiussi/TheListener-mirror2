#include "GPE/Interactable.h"
#include "Components/ShapeComponent.h"
#include "Components/BoxComponent.h"
#include "Miscellaneous/TLUtils.h"
#include "Player/LukaCharacter.h"

AInteractable::AInteractable(const FObjectInitializer& ObjectInitializer) : Super{ObjectInitializer}
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, "Mesh");
	check(StaticMeshComponent);
}

class UStaticMeshComponent* AInteractable::GetStaticMeshComponent() const
{
	return StaticMeshComponent;
}

bool AInteractable::Interact(AActor *Key)
{
	if (!bEnabled)
	{
		return false;
	}

	OnInteract();
	
	if (Keys.Num() != 0 && Keys.Contains(Key))
	{
		return true;
	}

	return false;
}

bool AInteractable::IsEnabled() const
{
	return bEnabled;
}

void AInteractable::Enable(bool bToggle)
{
	bEnabled = bToggle;
}

bool AInteractable::TestKey(AActor* Key)
{
	if (Key && Keys.Num() > 0)
	{
		return Keys.Contains(Key);
	}
	
	return Keys.Num() == 0;
}

void AInteractable::BeginPlay()
{
	Super::BeginPlay();

	check(StaticMeshComponent);
	StaticMeshComponent->SetEnableGravity(false);
	StaticMeshComponent->SetSimulatePhysics(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AInteractable::OnInteract()
{
	OnInteract_();
}

UInputMappingContext* AInteractable::GetInputMappingContext() const
{
	return InputMappingContext;
}
