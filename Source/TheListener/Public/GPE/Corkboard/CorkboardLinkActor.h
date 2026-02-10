// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CorkboardLinkActor.generated.h"

UCLASS()
class THELISTENER_API ACorkboardLinkActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACorkboardLinkActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	FString Clue1Name;
	UPROPERTY(EditAnywhere)
	FString Clue2Name;

	UPROPERTY()
	class ACorkboardNoteActor* Clue1Ptr;
	class ACorkboardNoteActor* Clue2Ptr;

public:
	bool HasThisClue(class ACorkboardNoteActor* Clue) const;
	class ACorkboardNoteActor* GetOtherClue(class ACorkboardNoteActor* Clue);

	void UpdateLink();

	class ACorkboardNoteActor* GetClue1() const;
	class ACorkboardNoteActor* GetClue2() const;

	FString GetClue1Name() const;
	FString GetClue2Name() const;

	void SetCluePtr(class ACorkboardNoteActor* Clue1, class ACorkboardNoteActor* Clue2);
	
};
