// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "GPE/InspectableComponent.h"

#include "Components/CanvasPanel.h"
#include "Player/LukaController.h"
#include "UI/InspectWidget.h"
#include "UI/LukaHUD.h"

UInspectableComponent::UInspectableComponent()
{
}

void UInspectableComponent::ShowWidget()
{
	if (InspectWidget)
	{
		InspectWidget->SetVisibility(ESlateVisibility::Visible);
	} else
	{
		if (ensure(InspectClass))
		{
			ALukaController *LukaController = Cast<ALukaController>(GetWorld()->GetFirstPlayerController());
			if (ALukaHUD * LukaHUD = Cast<ALukaHUD>(LukaController->GetHUD()))
			{
				UCanvasPanel *InspectablePanel = LukaHUD->GetInspectablePanel();
				check(InspectablePanel);
				FString WName = FString("Inspect_");
				WName += InspectClass->GetName();
				InspectWidget = Cast<UInspectWidget>(CreateWidget(InspectablePanel, InspectClass, FName(WName)));
				InspectablePanel->AddChildToCanvas(InspectWidget);

				InspectWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UInspectableComponent::HideWidget()
{
	if (ensure(InspectWidget))
	{
		InspectWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInspectableComponent::MoveSelector(FVector2D const& Direction)
{
	if (InspectWidget)
	{
		InspectWidget->MoveSelector(Direction);
	}
}

void UInspectableComponent::Select()
{
	if (InspectWidget)
	{
		InspectWidget->Select();
	}
}

void UInspectableComponent::BeginPlay()
{
	Super::BeginPlay();
}
