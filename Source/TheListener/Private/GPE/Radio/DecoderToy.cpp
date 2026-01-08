// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "GPE/Radio/DecoderToy.h"

#include "EnhancedInputComponent.h"
#include "GPE/Radio/Decoder.h"
#include "Player/LukaController.h"


ADecoderToy::ADecoderToy(const FObjectInitializer& ObjectInitializer)
{
	DecoderComponent = CreateDefaultSubobject<UChildActorComponent>("DecoderComponent");
	DecoderComponent->SetupAttachment(RootComponent);
	check(DecoderComponent);

	if (DecoderComponent)
	{
		DecoderComponent->SetChildActorClass(DecoderComponentClass);
		Decoder = Cast<ADecoder>(DecoderComponent->GetChildActor());
	}
}

// Called when the game starts or when spawned
void ADecoderToy::BeginPlay()
{
	Super::BeginPlay();
	if (!Decoder)
	{
		Decoder = Cast<ADecoder>(DecoderComponent->GetChildActor());
	}
	
}

// Called every frame
void ADecoderToy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ADecoderToy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (ensure(Decoder))
	{
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		Decoder->SetupPlayerInputComponent(EnhancedInputComponent);
	}
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void ADecoderToy::PossessedBy(AController* NewController)
{
	if (ALukaController* Luka = Cast<ALukaController>(NewController))
	{
		Controller = Luka;
		Decoder->StartDecoder(Luka);
		Lock(true);
	}
	Super::PossessedBy(NewController);
}

void ADecoderToy::UnPossessed()
{
	if (Controller)
	{
		Decoder->StopDecoder();
		Controller = nullptr;
	}
	Super::UnPossessed();
}



