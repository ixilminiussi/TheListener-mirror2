// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "GPE/Corkboard/CorkboardLinkActor.h"
#include "GPE/Corkboard/CorkboardNoteActor.h"

// Sets default values
ACorkboardLinkActor::ACorkboardLinkActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACorkboardLinkActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(true);
	//ClueLinked1Ptr = Cast<ACorkboardNoteActor>(ClueLinked1->GetChildActor());
	//ClueLinked2Ptr = Cast<ACorkboardNoteActor>(ClueLinked2->GetChildActor());
}

ACorkboardNoteActor* ACorkboardLinkActor::GetOtherClue(ACorkboardNoteActor* Clue) 
{
	if (Clue == Clue1Ptr) {return Clue2Ptr;}
	else if (ensureAlways(Clue == Clue2Ptr)) {return Clue1Ptr;} //Should not trigger ensure
	else {return nullptr;}
}

void ACorkboardLinkActor::UpdateLink()
{
	bool bIsVisible = Clue1Ptr->IsNoteVisible() && Clue2Ptr->IsNoteVisible();
	SetActorHiddenInGame(!bIsVisible);
}

bool ACorkboardLinkActor::HasThisClue( ACorkboardNoteActor* Clue) const
{
	return Clue == Clue1Ptr || Clue == Clue2Ptr;
}

ACorkboardNoteActor* ACorkboardLinkActor::GetClue1() const
{
	return Clue1Ptr;
}

ACorkboardNoteActor* ACorkboardLinkActor::GetClue2() const
{
	return Clue2Ptr;
}

FString ACorkboardLinkActor::GetClue1Name() const
{
	return Clue1Name;
}

FString ACorkboardLinkActor::GetClue2Name() const
{
	return Clue2Name;
}

void ACorkboardLinkActor::SetCluePtr(class ACorkboardNoteActor* Clue1, class ACorkboardNoteActor* Clue2)
{
	Clue1Ptr = Clue1;
	Clue2Ptr = Clue2;
}


