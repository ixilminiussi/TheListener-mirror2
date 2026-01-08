// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GPE/Toy.h"
#include "DecoderToy.generated.h"

UCLASS()
class THELISTENER_API ADecoderToy : public AToy
{
	GENERATED_BODY()

public:
	ADecoderToy(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Toy|Radio|Decoder")
	TObjectPtr<class UChildActorComponent> DecoderComponent{nullptr};
	UPROPERTY()
	TObjectPtr<class ADecoder> Decoder{nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio|Decoder")
	TSubclassOf<class ADecoder> DecoderComponentClass;

	TObjectPtr<class ALukaController> Controller;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	
};
