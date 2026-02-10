// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "GPE/Corkboard/CorkboardNoteActor.h"


// Sets default values
ACorkboardNoteActor::ACorkboardNoteActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACorkboardNoteActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(true);
	
}

void ACorkboardNoteActor::EnableNote(bool bNewState)
{
	if (CurrentState == EClueState::Missing || CurrentState == EClueState::Disabled) {return;}
	SetActorHiddenInGame(!bNewState);
	if (bNewState)
	{
		for (TWeakObjectPtr<ACorkboardNoteActor> LinkedNote: LinkedNotes)
		{
			if (!LinkedNote.Get()->IsNoteVisible())
			{
				CurrentState = EClueState::Partial;
			}
		}
		CurrentState = EClueState::Complete;
	}
	else
	{
		CurrentState = EClueState::Disabled;
		for (TWeakObjectPtr<ACorkboardNoteActor> LinkedNote: LinkedNotes)
		{
			if (LinkedNote->IsNoteVisible())
			{
				LinkedNote->SetMissingState();
			}
		}
	}
}

bool ACorkboardNoteActor::IsNoteVisible() const
{
	return CurrentState == EClueState::Complete || CurrentState == EClueState::Partial || CurrentState == EClueState::Missing;
}

void ACorkboardNoteActor::AddLinkedNote(TWeakObjectPtr<ACorkboardNoteActor> Note)
{
	LinkedNotes.Add(Note);
}

void ACorkboardNoteActor::SetMissingState()
{
	CurrentState = EClueState::Missing;
}

