// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CorkboardNoteActor.generated.h"

UENUM()
enum class EClueState : uint8
{
	Hidden,
	Partial,
	Complete,
	Disabled,
	Missing
};

UCLASS()
class THELISTENER_API ACorkboardNoteActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACorkboardNoteActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	EClueState CurrentState = EClueState::Hidden;

	TArray<TWeakObjectPtr<ACorkboardNoteActor>> LinkedNotes;
	
public:
	UFUNCTION(BlueprintCallable)
	virtual void EnableNote(bool bNewState);

	bool IsNoteVisible() const;

	void AddLinkedNote(TWeakObjectPtr<ACorkboardNoteActor> Note);

	void SetMissingState();
};
