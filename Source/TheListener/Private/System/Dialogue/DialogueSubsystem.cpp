// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Dialogue/DialogueSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "GPE/Phone.h"
#include "Inkpot/Inkpot.h"
#include "Inkpot/InkpotStory.h"
#include "Kismet/GameplayStatics.h"
#include "Platforms/AkPlatform_Windows/AkWindowsInitializationSettings.h"
#include "System/Core/ListenerWorldSettings.h"
#include "System/Events/EventCondition.h"
#include "EventSubsystem.h"
#include "UI/Dialogue/AnswerWidget.h"

bool UDialogueSubsystem::ShouldCreateSubsystem(UObject* Outer) const
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

void UDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UWorld* World = GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return;
	}
	const AListenerWorldSettings* WorldSettings = Cast<AListenerWorldSettings>(World->GetWorldSettings());
	if (!WorldSettings)
	{
		return;
	}

	FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UDialogueSubsystem::OnActorsInitialized);
	UE_LOG(LogTemp, Log, TEXT("UDialogueSubsystem Initialized"));
}

void UDialogueSubsystem::OnActorsInitialized(const FActorsInitializedParams& Params)
{
	UInkpot* InkpotSubsystem = GEngine->GetEngineSubsystem<UInkpot>();
	InkpotSubsystem->EventOnStoryBegin.AddDynamic(this, &UDialogueSubsystem::OnStoryBegin);

	AActor* PhoneActor = UGameplayStatics::GetActorOfClass(GetWorld(), APhone::StaticClass());
	Phone = Cast<APhone>(PhoneActor);
	ensure(Phone);
}

void UDialogueSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UDialogueSubsystem::OnSelectChoice(uint32 ChoiceID, const uint32 Choice)
{
	CurrentStory->ChooseChoiceIndex(Choice);
	Continue();
}

void UDialogueSubsystem::SetText()
{
	// CurrentDialogueWidget->SetDialogueText(FText::FromString(CurrentStory->GetCurrentText()));
	// CurrentDialogueWidget->HideAllButtons();
	// int index = 0;
	// for (UInkpotChoice* Choice : CurrentStory->GetCurrentChoices())
	// {
	// 	CurrentDialogueWidget->SetButtonText(Choice->GetText(), index);
	// 	CurrentDialogueWidget->SetButtonVisibility(ESlateVisibility::Visible, index);
	// 	index++;
	// }
}

void UDialogueSubsystem::Continue()
{
	if (CurrentStory->CanContinue())
	{
		CurrentStory->Continue();
		return;
	}
	FinishStory();
}

UInkpotStory* UDialogueSubsystem::StartStory(UInkpotStoryAsset* NewStory)
{
	UInkpot* InkpotSubsystem = GEngine->GetEngineSubsystem<UInkpot>();
	if (CurrentStory != nullptr) { InkpotSubsystem->EndStory(CurrentStory); }
	CurrentStory = InkpotSubsystem->BeginStory(NewStory);
	return CurrentStory;
}

void UDialogueSubsystem::OnStoryBegin(UInkpotStory* NewStory)
{
	CurrentStory = NewStory;
	NewStory->OnContinue().AddDynamic(this, &UDialogueSubsystem::OnStoryContinue);
	
	// CurrentDialogueWidget->SetVisibility(ESlateVisibility::Visible);
	FInputModeUIOnly Input;
	// Input.SetWidgetToFocus(CurrentDialogueWidget->TakeWidget());
	GetWorld()->GetFirstPlayerController()->SetInputMode(Input);
	Continue();
}

void UDialogueSubsystem::OnStoryContinue(UInkpotStory* Story)
{
	// SetChoicesVisibility(Story->HasChoices());
	SetText();
	CheckTags();
}

void UDialogueSubsystem::FinishStory()
{
	EventOnDialogueEnd.Broadcast(CurrentStory);
	// CurrentDialogueWidget->SetVisibility(ESlateVisibility::Collapsed);
	FInputModeGameOnly Input;
	GetWorld()->GetFirstPlayerController()->SetInputMode(Input);
	CurrentStory = nullptr;
}

class APhone* UDialogueSubsystem::GetPhone()
{
	check(Phone);
	return Phone;
}

void UDialogueSubsystem::CheckTags()
{
	TArray<FString> Tags = CurrentStory->GetCurrentTags();
	for (FString Tag : Tags)
	{
		FString TagCmd = Tag.LeftChop(Tag.Len() - 3);
		FString TagArgs = Tag.RightChop(3);
		if (TagCmd == "CD_")
		{
			bool EnableCondition = (TagArgs.LeftChop(Tag.Len() - 1)) != "!";
			FConditionKey ConditionKey = UInkpotCondition::GenerateKey(TagArgs);
			GetWorld()->GetSubsystem<UEventSubsystem>()->SetConditionValue(ConditionKey, EnableCondition, true);
			return;
		}
	}
}
