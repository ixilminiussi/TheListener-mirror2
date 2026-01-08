// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/InspectingComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "GPE/File.h"
#include "GPE/Toy.h"
#include "Player/LukaCharacter.h"
#include "Player/LukaController.h"
#include "Rendering/StaticLightingSystemInterface.h"
#include "UI/InspectWidget.h"
#include "UI/Prompt/CommandHUDComponent.h"

class UEnhancedInputLocalPlayerSubsystem;

UInspectingComponent::UInspectingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CommandHUDComponent = CreateDefaultSubobject<UCommandHUDComponent>("CommandHUD");
}

UInputMappingContext* UInspectingComponent::GetInputMappingContext() const
{
	return InputMappingContext;
}

void UInspectingComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const ALukaController* LukaController = Cast<ALukaController>(GetOwner()); ensure(LukaController))
	{
		LukaCharacter = LukaController->GetLukaCharacter();
		check(LukaCharacter);
	}

	check(CommandHUDComponent);
	CommandHUDComponent->Generate(InputMappingContext);
}

void UInspectingComponent::ReadFile(TScriptInterface<IInspectable> InspectableToRead)
{
	check(InspectableToRead.GetInterface() != nullptr);
	
	if (Inspectable.GetInterface() != nullptr)
	{
		if (Inspectable->GetInspectWidget()->IsVisible())
		{
			return;
		}

		Inspectable->HideInspectable();
		Inspectable.SetInterface(nullptr);
	}
	Inspectable = InspectableToRead;
	if (WeakInputLocalPlayerSubsystem.IsValid())
	{
		check(LukaCharacter);
		WeakInputLocalPlayerSubsystem.Get()->RemoveMappingContext(LukaCharacter->GetLukaMappingContext());
		WeakInputLocalPlayerSubsystem.Get()->AddMappingContext(InputMappingContext, 1);
	}

	
	if (Inspectable.GetInterface() != nullptr)
	{
		Inspectable->ShowInspectable();

		if (const AObji* Obji = LukaCharacter->GetHeldObji(); Obji != nullptr)
		{
			Obji->SetActiveWidgetVisibility(false);
		}
		if (const AToy* Toy = Cast<AToy>(GetWorld()->GetFirstPlayerController()->GetPawn()); Toy != nullptr)
		{
			Toy->SetActiveWidgetVisibility(false);
		}

		CommandHUDComponent->AddActiveToHUD();
	}
}

void UInspectingComponent::MoveSelector(const struct FInputActionInstance& Value)
{
	if (Inspectable.GetInterface() != nullptr)
	{
		UInspectWidget* CurrentWidget = Inspectable->GetInspectWidget();
		CurrentWidget->MoveSelector(Value.GetValue().Get<FVector2D>());
	}
}

void UInspectingComponent::UseSelector(const struct FInputActionInstance&)
{
	if (Inspectable.GetInterface() != nullptr)
	{
		UInspectWidget* CurrentWidget = Inspectable->GetInspectWidget();
		CurrentWidget->Select();
	}
}

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

	if (const AObji* Obji = LukaCharacter->GetHeldObji(); Obji != nullptr)
	{
		Obji->SetActiveWidgetVisibility(true);
	}
	if (const AToy* Toy = Cast<AToy>(GetWorld()->GetFirstPlayerController()->GetPawn()); Toy != nullptr)
	{
		Toy->SetActiveWidgetVisibility(true);
	}
	CommandHUDComponent->RemoveActiveFromHUD();
}

void UInspectingComponent::Read(const FInputActionInstance& Instance)
{
	check(LukaCharacter);

	
	if (IInspectable* NewInspectable = Cast<IInspectable>(LukaCharacter->GetHeldObji()); NewInspectable != nullptr)
	{
		ReadFile(TScriptInterface<IInspectable>(LukaCharacter->GetHeldObji()));
		return;
	}

	if (IInspectable* NewInspectable = Cast<IInspectable>(LukaCharacter->GetInteractableInView()); NewInspectable != nullptr)
	{
		ReadFile(TScriptInterface<IInspectable>(LukaCharacter->GetInteractableInView()));
	}
}

void UInspectingComponent::SetupInput(UEnhancedInputComponent* EnhancedInputComponent,
                                      UEnhancedInputLocalPlayerSubsystem* InputLocalPlayerSubsystem)
{
	check(InputLocalPlayerSubsystem);
	check(EnhancedInputComponent);

	WeakInputLocalPlayerSubsystem = TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem>(InputLocalPlayerSubsystem);

	if (ensure(InputActionLeave != nullptr))
	{
		EnhancedInputComponent->BindAction(InputActionLeave, ETriggerEvent::Triggered, this,
		                                   &UInspectingComponent::Leave);
	}
	if (ensure(InputActionRead != nullptr))
	{
		EnhancedInputComponent->BindAction(
			InputActionRead, ETriggerEvent::Triggered, this, &UInspectingComponent::Read);
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
