// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Clues/ClueSubsystem.h"

#include "Engine/ObjectLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GPE/CorkboardDiegetic.h"
#include "System/Core/ListenerWorldSettings.h"
#include "UI/LukaHUD.h"

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

	FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UClueSubsystem::OnActorsInitialized);

	UE_LOG(LogTemp, Log, TEXT("UFrequencySubsystem Initialized"));
}

void UClueSubsystem::OnActorsInitialized(const FActorsInitializedParams& Params)
{
	Corkboard = Cast<ACorkboardDiegetic>(UGameplayStatics::GetActorOfClass(GetWorld(), ACorkboardDiegetic::StaticClass()));

	if (!ensure(Corkboard))
	{
		UE_LOG(LogTemp, Warning, TEXT("Corkboard not found !"));
		return;
	}
	CluesList = TArray<FString>(Corkboard->GetClues());
}

void UClueSubsystem::GiveClue(FString ClueName)
{
	if (!Corkboard)
	{
		return;
	}
	if (FoundClues.Contains(ClueName)) {return;}
	FoundClues.AddUnique(ClueName);
	Corkboard->UpdateClue(ClueName,true);
	ALukaHUD* HUD = Cast<ALukaHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	HUD->NotifyClue();

#if UE_EDITOR
	//DisplayClues();
#endif
}

void UClueSubsystem::RemoveClue(FString Clue)
{
	Corkboard->UpdateClue(Clue,false);
	CluesList.Remove(Clue);
	FoundClues.Remove(Clue);

#if UE_EDITOR
	//DisplayClues();
#endif
}

