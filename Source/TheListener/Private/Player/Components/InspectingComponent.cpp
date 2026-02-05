// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/Components/InspectingComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GPE/InspectableComponent.h"
#include "Miscellaneous/TLUtils.h"
#include "Player/Components/PickupComponent.h"

class UEnhancedInputLocalPlayerSubsystem;

UInspectingComponent::UInspectingComponent()
{
}

void UInspectingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInspectingComponent::OnRead()
{
	if (UPickupComponent *PickupComponent = GetOwner()->FindComponentByClass<UPickupComponent>())
	{
		TryRead(PickupComponent->GetHeld());
	}
}

void UInspectingComponent::TryRead(AActor *Actor)
{
	if (!Actor)
	{
		return;
	}
	
	UInspectableComponent *InspectableComponent = Actor->FindComponentByClass<UInspectableComponent>();

	if (InspectableComponent)
	{
		InspectableComponent->ShowWidget();

		ULocalPlayer *LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		if (LocalPlayer)
		{
			UEnhancedInputLocalPlayerSubsystem *InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

			InputSubsystem->AddMappingContext(InputMappingContext, 5);

			UTLUtils::TogglePrompts(GetWorld(), ActivePrompts, true);
		}
	}

	Inspectable = Actor;
}

void UInspectingComponent::TryLeave()
{
	if (!Inspectable)
	{
		return;
	}
	
	UInspectableComponent *InspectableComponent = Inspectable->FindComponentByClass<UInspectableComponent>();

	Inspectable = nullptr;
	
	if (InspectableComponent)
	{
		InspectableComponent->HideWidget();

		ULocalPlayer *LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		if (LocalPlayer)
		{
			UEnhancedInputLocalPlayerSubsystem *InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

			InputSubsystem->RemoveMappingContext(InputMappingContext);
		}
		UTLUtils::TogglePrompts(GetWorld(), ActivePrompts, false);
	}
}

void UInspectingComponent::MoveSelector(const struct FInputActionInstance& Value)
{
	if (!Inspectable)
	{
		return;
	}
	
	UInspectableComponent *InspectableComponent = Inspectable->FindComponentByClass<UInspectableComponent>();

	if (InspectableComponent)
	{
		InspectableComponent->MoveSelector(Value.GetValue().Get<FVector2D>());
	}
}

void UInspectingComponent::UseSelector(const struct FInputActionInstance&)
{
	UInspectableComponent *InspectableComponent = Inspectable->FindComponentByClass<UInspectableComponent>();

	if (InspectableComponent)
	{
		InspectableComponent->Select();
	}
}

	/*
void UInspectingComponent::Leave(const FInputActionInstance& Instance)
{
	if (Inspectable.GetInterface() != nullptr)
	{
		if (Inspectable->IsInspectableVisible())
		{
			Inspectable->HideInspectable();

			if (ensure(WeakInputLocalPlayerSubsystem.IsValid()))
			{
				check(LukaCharacter);
				WeakInputLocalPlayerSubsystem.Get()->RemoveMappingContext(InputMappingContext);
				WeakInputLocalPlayerSubsystem.Get()->AddMappingContext(LukaCharacter->GetLukaMappingContext(), 1);
			}
		}
	}

	UTLUtils::TogglePrompt(GetWorld(), {ActivePrompt}, false);
}
*/

	/*
void UInspectingComponent::Read(const FInputActionInstance& Instance)
{
}
*/

void UInspectingComponent::SetupInput(UEnhancedInputComponent* EnhancedInputComponent)
{
	check(EnhancedInputComponent);

	if (ensure(InputActionReturn != nullptr))
	{
		EnhancedInputComponent->BindAction(InputActionReturn, ETriggerEvent::Triggered, this,
										   &UInspectingComponent::TryLeave);
	}
	if (ensure(InputActionInspect != nullptr))
	{
		EnhancedInputComponent->BindAction(
			InputActionInspect, ETriggerEvent::Triggered, this, &UInspectingComponent::OnRead);
	}
	if (ensure(InputActionSelectorMove != nullptr))
	{
		EnhancedInputComponent->BindAction(
			InputActionSelectorMove, ETriggerEvent::Triggered, this, &UInspectingComponent::MoveSelector);
		EnhancedInputComponent->BindAction(
			InputActionSelectorMove, ETriggerEvent::Completed, this, &UInspectingComponent::MoveSelector);
	}
	if (ensure(InputActionSelect != nullptr))
	{
		EnhancedInputComponent->BindAction(
			InputActionSelect, ETriggerEvent::Started, this, &UInspectingComponent::UseSelector);
	}
}
