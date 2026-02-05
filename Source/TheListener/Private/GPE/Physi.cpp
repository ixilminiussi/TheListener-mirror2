// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.
#include "GPE/Physi.h"

#include "Components/TimelineComponent.h"

/*
void APhysi::BeginPlay()
{
	Super::BeginPlay();
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetEnableGravity(true);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

AObji* APhysi::CreatePreview() const
{
	return nullptr;
}

bool APhysi::Drop(FVector3d const &From, FVector3d const &Impulse)
{
	FTimeline* MoveTimeline = MoveTo(From);
	FTimeline* RotateTimeline = RotateTo(FRotator{});

	const auto LambdaFunc = FOnTimelineEventStatic::CreateLambda([this, Impulse]
	{
		OnDrop();
		StaticMeshComponent->AddImpulse(Impulse);
	});
	if (MoveTimeline->GetTimelineLength() > RotateTimeline->GetTimelineLength())
	{
		MoveTimeline->SetTimelineFinishedFunc(LambdaFunc);
	}
	else
	{
		RotateTimeline->SetTimelineFinishedFunc(LambdaFunc);
	}

	return true;
}
*/