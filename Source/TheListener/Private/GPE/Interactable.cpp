#include "GPE/Interactable.h"
#include "Components/BoxComponent.h"
#include "Player/LukaCharacter.h"
#include "Player/LukaController.h"
#include "UI/Prompt/CommandHUDComponent.h"

AInteractable::AInteractable(const FObjectInitializer& ObjectInitializer) : Super{ObjectInitializer}
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, "Mesh");
	CollisionComponent = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, "Collision");

	SetRootComponent(StaticMeshComponent);
	CollisionComponent->SetupAttachment(StaticMeshComponent);

	check(CollisionComponent);
	CollisionComponent->SetCollisionProfileName(FName("Hitbox"));

	check(StaticMeshComponent);

	CommandHUDComponent = CreateDefaultSubobject<UCommandHUDComponent>(TEXT("CommandHUDComponent"));
}

class UStaticMeshComponent* AInteractable::GetStaticMeshComponent() const
{
	return StaticMeshComponent;
}

void AInteractable::Interact(const class ALukaCharacter* Luka)
{
	if (!bEnabled)
	{
		return;
	}

	check(Luka);
	OnInteract(Luka->GetController());
}

bool AInteractable::IsEnabled() const
{
	return bEnabled;
}

void AInteractable::Enable(bool bToggle)
{
	bEnabled = bToggle;
}

void AInteractable::BeginPlay()
{
	Super::BeginPlay();

	check(StaticMeshComponent);
	StaticMeshComponent->SetEnableGravity(false);
	StaticMeshComponent->SetSimulatePhysics(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	check(CommandHUDComponent);
	CommandHUDComponent->Generate(InputMappingContext);
}

void AInteractable::OnInteract(class AController* NewController)
{
	check(NewController);
	LukaController = Cast<ALukaController>(NewController);
	check(LukaController);

	OnInteract_();
}

UInputMappingContext* AInteractable::GetInputMappingContext() const
{
	return InputMappingContext;
}

void AInteractable::SetHoverWidgetVisibility(const bool bIsVisible) const
{
	check(CommandHUDComponent);
	if (bIsVisible)
	{
		CommandHUDComponent->AddHoverToHUD();
	}
	else
	{
		CommandHUDComponent->RemoveHoverFromHUD();
	}
}

void AInteractable::SetActiveWidgetVisibility(const bool bIsVisible) const
{
	check(CommandHUDComponent);
	if (bIsVisible)
	{
		CommandHUDComponent->AddActiveToHUD();
	}
	else
	{
		CommandHUDComponent->RemoveActiveFromHUD();
	}
}
