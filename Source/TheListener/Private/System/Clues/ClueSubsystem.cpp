// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Clues/ClueSubsystem.h"

#include "Engine/ObjectLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "System/Clues/ClueAsset.h"
#include "GPE/Corkboard.h"
#include "System/Core/ListenerWorldSettings.h"
#include "System/Clues/ClueSubsystemData.h"

bool UClueSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const auto World = Cast<UWorld>(Outer);
	if (!World)
	{
		return false;
	}
	if (World->IsGameWorld())
	{
		if (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE)
		{
			AListenerWorldSettings* WorldSettings = Cast<AListenerWorldSettings>(World->GetWorldSettings());
			if (!WorldSettings)
			{
				return false;
			}

			const auto AllowedLevels = WorldSettings->AllowedLevels;

			for (const auto& Level : AllowedLevels)
			{
				if (Level.IsValid() && !Level.ToSoftObjectPath().IsValid())
				{
					continue;
				}

				if (World->GetName() == Level.GetAssetName())
				{
					return true;
				}
			}
		}
	}
	return false;
}

void UClueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	auto World = GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return;
	}
	AListenerWorldSettings* WorldSettings = Cast<AListenerWorldSettings>(World->GetWorldSettings());
	if (!WorldSettings)
	{
		return;
	}

	if (UClueSubsystemData* ClueSubsystemData = WorldSettings->ClueSubsystemData.LoadSynchronous())
	{
		this->CluesList = ClueSubsystemData->CluesList;
	}

	FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UClueSubsystem::OnActorsInitialized);

	UE_LOG(LogTemp, Log, TEXT("UFrequencySubsystem Initialized"));
}

void UClueSubsystem::OnActorsInitialized(const FActorsInitializedParams& Params)
{
	Corkboard = Cast<ACorkboard>(UGameplayStatics::GetActorOfClass(GetWorld(), ACorkboard::StaticClass()));

	if (!ensure(Corkboard)) { UE_LOG(LogTemp, Warning, TEXT("Corkboard not found !")); }
	for (UClueAsset* Clue : CluesList)
	{
		Clue->ResetClueState();
	}
}

void UClueSubsystem::GiveClue(UClueAsset* Clue)
{
	FoundClues.AddUnique(Clue);
	Clue->CheckClueState(true);
	for (UClueAsset* Link : Clue->Links)
	{
		Link->CheckClueState(false);
	}
	Corkboard->UpdateClue(Clue);
#if UE_EDITOR
	DisplayClues();
#endif
}

void UClueSubsystem::RemoveClue(class UClueAsset* Clue)
{
	Clue->MakeUnavailable();
	for (UClueAsset* Link : Clue->Links)
	{
		Link->CheckClueState(false);
	}
	Corkboard->UpdateClue(Clue);
	CluesList.Remove(Clue);
	FoundClues.Remove(Clue);

#if UE_EDITOR
	DisplayClues();
#endif
}

#if UE_EDITOR
void UClueSubsystem::DisplayClues()
{
	UE_LOG(LogTemp, Log, TEXT("Displaying Clues : "));
	for (UClueAsset* Clue : CluesList)
	{
		UE_LOG(LogTemp, Log, TEXT("Clue Name : %s [%s]"), *Clue->Name, *UEnum::GetValueAsString(Clue->State));
		UE_LOG(LogTemp, Log, TEXT(" | Hints Found : "));
		if (Clue->Links.Num() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT(" | Links : "));
			for (UClueAsset* LinkedClue : Clue->Links)
			{
				UE_LOG(LogTemp, Log, TEXT(" | - %s [%s]"), *LinkedClue->Name,
				       *UEnum::GetValueAsString(LinkedClue->State));
			}
		}
	}
	UE_LOG(LogTemp, Log, TEXT("==========================================================="));
}
#endif
