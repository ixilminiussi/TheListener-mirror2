// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "GPE/CorkboardDiegetic.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "GPE/Corkboard/CorkboardLinkActor.h"
#include "GPE/Corkboard/CorkboardNoteActor.h"
#include "DrawDebugHelpers.h"


// Sets default values
ACorkboardDiegetic::ACorkboardDiegetic()
{
	PrimaryActorTick.bCanEverTick = true;
	BoardMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Board Mesh");
	check(BoardMeshComponent);

	BoardMeshComponent->SetupAttachment(CollisionComponent);

	CameraPlaneMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Camera Plane");
	check(CameraPlaneMeshComponent);

	CameraPlaneMeshComponent->SetupAttachment(CollisionComponent);

	DiscoverTimelineComponent = CreateDefaultSubobject<UTimelineComponent>("DiscoverTimeline");
}

// Called when the game starts or when spawned
void ACorkboardDiegetic::BeginPlay()
{
	Super::BeginPlay();

	float MinTime;
	float MaxTime;
	DiscoverInterpFunction.BindUFunction(this, FName("ProgressDiscoverTimeline"));
	DiscoverFinishedFunction.BindUFunction(this, FName("FinishDiscoverTimeline"));
	DiscoverTimelineComponent->AddInterpFloat(DiscoverTimelineCurve,DiscoverInterpFunction);
	DiscoverTimelineCurve->GetTimeRange(MinTime, MaxTime);
	DiscoverTimelineComponent->SetTimelineLength(MaxTime);
	DiscoverTimelineComponent->SetTimelineFinishedFunc(DiscoverFinishedFunction);
	DiscoverTimelineComponent->SetLooping(false);

	FVector CameraMeshScaledLocalBounds = CameraPlaneMeshComponent->GetStaticMesh()->GetBounds().BoxExtent ;
	double XBounds = CameraMeshScaledLocalBounds.X;
	double YBounds = CameraMeshScaledLocalBounds.Y;
	PlaneWidth = XBounds * CameraPlaneMeshComponent->GetRelativeScale3D().X * 2;
	PlaneHeight = YBounds * CameraPlaneMeshComponent->GetRelativeScale3D().Y * 2;

	LocationDL = CameraPlaneMeshComponent->GetComponentTransform().TransformPosition(FVector(-XBounds,-YBounds,0));
	LocationUL = CameraPlaneMeshComponent->GetComponentTransform().TransformPosition(FVector(-XBounds,YBounds,0));
	LocationDR = CameraPlaneMeshComponent->GetComponentTransform().TransformPosition(FVector(XBounds,-YBounds,0));
	LocationUR = CameraPlaneMeshComponent->GetComponentTransform().TransformPosition(FVector(XBounds,YBounds,0));

	//We will not use UR for computing the normal
	FVector SubtractLocation1 =  LocationUL - LocationDL;
	FVector SubtractLocation2 =  LocationDR - LocationDL;

	PlaneNormal = FVector::CrossProduct(SubtractLocation1,SubtractLocation2);
	PlaneNormal.Normalize() ;

	PlaneDistance = (PlaneNormal.X * LocationDL.X)+ (PlaneNormal.Y * LocationDL.Y) +(PlaneNormal.Z * LocationDL.Z);

	PlaneAxisX = CameraPlaneMeshComponent->GetForwardVector();
	PlaneAxisY = CameraPlaneMeshComponent->GetRightVector();

	#if UE_EDITOR
	DrawDebugPoint(
		GetWorld(),
		LocationDL,
		10,
		FColor::Red,
		true,
		-1);
	DrawDebugPoint(
		GetWorld(),
		LocationUL,
		10,
		FColor::Red,
		true,
		-1);
	DrawDebugPoint(
		GetWorld(),
		LocationDR,
		10,
		FColor::Red,
		true,
		-1);
	DrawDebugPoint(
		GetWorld(),
		LocationUR,
		10,
		FColor::Red,
		true,
		-1);
	DrawDebugLine(
		GetWorld(),
		LocationDL,
		LocationDR,
		FColor::Red,
		true,
		-1);
	DrawDebugLine(
		GetWorld(),
		LocationUR,
		LocationDR,
		FColor::Red,
		true,
		-1);
	DrawDebugLine(
		GetWorld(),
		LocationUR,
		LocationUL,
		FColor::Red,
		true,
		-1);
	DrawDebugLine(
		GetWorld(),
		LocationDL,
		LocationUL,
		FColor::Red,
		true,
		-1);
	#endif

	TArray<UChildActorComponent*> Components;
	GetComponents<UChildActorComponent>(Components);
	for (UChildActorComponent* Component : Components)
	{
		AActor* ChildActor = Component->GetChildActor();
		if (ACorkboardNoteActor* NoteActor = Cast<ACorkboardNoteActor>(ChildActor))
		{
			ClueActorsMap.Add(Component->GetName(), NoteActor);
			continue;
		}
		if (ACorkboardLinkActor* LinkActor = Cast<ACorkboardLinkActor>(ChildActor))
		{
			if (!ensure(ClueActorsMap.Contains(LinkActor->GetClue1Name()))){continue;}
			ACorkboardNoteActor* Clue1 = ClueActorsMap[LinkActor->GetClue1Name()].Get();
			if (!ensure(ClueActorsMap.Contains(LinkActor->GetClue2Name()))){continue;}
			ACorkboardNoteActor* Clue2 = ClueActorsMap[LinkActor->GetClue2Name()].Get();
			LinkActor->SetCluePtr(Clue1,Clue2);
			ClueLinkedMap.FindOrAdd(LinkActor->GetClue1()).Add(LinkActor );
			ClueLinkedMap.FindOrAdd(LinkActor->GetClue2()).Add(LinkActor );
			
			Clue1->AddLinkedNote(Clue2);
			Clue2->AddLinkedNote(Clue1);
		}
	}
}

// Called to bind functionality to input
void ACorkboardDiegetic::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (!ensure(EnhancedInputComponent != nullptr)) { return; }
	if (ensure(InputActionMove != nullptr))
	{
		EnhancedInputComponent->BindAction(InputActionMove, ETriggerEvent::Triggered, this,
										   &ACorkboardDiegetic::MoveInput);
		EnhancedInputComponent->BindAction(InputActionMove, ETriggerEvent::None, this,
										   &ACorkboardDiegetic::MoveInput);
	}
	if (ensure(InputActionZoom != nullptr))
	{
		EnhancedInputComponent->BindAction(InputActionZoom, ETriggerEvent::Triggered, this,
										   &ACorkboardDiegetic::ZoomInput);
		EnhancedInputComponent->BindAction(InputActionZoom
			, ETriggerEvent::None, this,
										   &ACorkboardDiegetic::ZoomInput);
	}
}

void ACorkboardDiegetic::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!IsLocked())
	{
		//Camera Zoom Part
		CurrentZoomScale -= LastZoomInputValue*(DeltaSeconds*ZoomScaleSpeed);
		CurrentZoomScale = FMath::Clamp(CurrentZoomScale, 0.f, 1.f);

		//Camera Move Part
		float CurrentMoveSpeed = CamSpeedCurve->GetFloatValue(CurrentZoomScale);
		FVector2D MoveValue = LastMoveInputValue*DeltaSeconds*CurrentMoveSpeed;
		CurrentPosition += MoveValue;
		CurrentPosition.X = FMath::Clamp(CurrentPosition.X, 0.f, 1.f);
		CurrentPosition.Y = FMath::Clamp(CurrentPosition.Y, 0.f, 1.f);
		
		FVector XLerpDown = FMath::Lerp(LocationDL,LocationDR,CurrentPosition.X);
		FVector XLerpUp = FMath::Lerp(LocationUL,LocationUR,CurrentPosition.X);
		FVector NewPosition = FMath::Lerp(XLerpDown,XLerpUp,CurrentPosition.Y);
		CameraComponent.Get()->SetWorldLocation(NewPosition);
	}
	float CurrentFOV = FOVCurve->GetFloatValue(CurrentZoomScale);
	CameraComponent.Get()->SetFieldOfView(CurrentFOV);
}

void ACorkboardDiegetic::OnPossessAfterTransition_Implementation()
{
	Super::OnPossessAfterTransition_Implementation();
	
	if (!CluesUpdated.IsEmpty())
	{
		StartDiscoverTimeline();
	}
	
	
}

void ACorkboardDiegetic::OnUnPossessAfterTransition_Implementation()
{
	Super::OnUnPossessAfterTransition_Implementation();
	CurrentPosition = FVector2D(0.5f,0.5f);
	CurrentZoomScale = 0.5f;
}

FVector ACorkboardDiegetic::FindClosestPositionOnPlane(FVector Point)
{
	float Dist = FVector::DotProduct(Point, PlaneNormal) - PlaneDistance ;
	return Point - Dist * PlaneNormal;
}

FVector2D ACorkboardDiegetic::FindPositionOnPlaneByRatio(FVector Point)
{
	float DistDL = FVector::Dist(LocationDL,Point);
	float DistDR = FVector::Dist(LocationDR,Point); 
	float DistUL = FVector::Dist(LocationUL,Point); 
	float DistUR = FVector::Dist(LocationUR,Point);
	return FVector2D(0,0);
}

void ACorkboardDiegetic::UpdateClue(FString ClueName, bool bActivate)
{
	if (!ensureAlways(ClueActorsMap.Contains(ClueName))) {return;} //No clue found with the same name
	ACorkboardNoteActor* NoteActor = ClueActorsMap[ClueName].Get();
	NoteActor->EnableNote(bActivate);
	for (TWeakObjectPtr<ACorkboardLinkActor> LinkActor : ClueLinkedMap[NoteActor] )
	{
		LinkActor.Get()->UpdateLink();
	}
	if (bActivate)
	{
		CluesUpdated.Enqueue(ClueName);
	}
}

void ACorkboardDiegetic::MoveInput(const struct FInputActionValue& Value)
{
	LastMoveInputValue = Value.Get<FVector2D>();
}

void ACorkboardDiegetic::ZoomInput(const struct FInputActionValue& Value)
{
	LastZoomInputValue = Value.Get<float>();
}


TArray<FString> ACorkboardDiegetic::GetClues()
{
	TArray<FString> CluesArray;
	for (TPair<FString,TWeakObjectPtr<ACorkboardNoteActor>> MapValue: ClueActorsMap)
	{
		CluesArray.Add(MapValue.Key);
	}
	return CluesArray;
}

void ACorkboardDiegetic::StartDiscoverTimeline()
{
	CluesUpdated.Dequeue(CurrentDiscoveringClue);
	Lock(true);
	
	ACorkboardNoteActor* CurrentClueActor = ClueActorsMap[CurrentDiscoveringClue].Get();
	DiscoverStartLocation = CameraComponent.Get()->GetComponentLocation();
	UChildActorComponent* ClueComponent = Cast<UChildActorComponent>(CurrentClueActor->GetRootComponent()->GetAttachParent());
	DiscoverEndLocation = FindClosestPositionOnPlane(ClueComponent->GetComponentLocation());
	DiscoverTimelineComponent->PlayFromStart();
}

void ACorkboardDiegetic::ProgressDiscoverTimeline(float value)
{
	FVector NewLocation = FMath::Lerp(DiscoverStartLocation, DiscoverEndLocation, value);
	CameraComponent.Get()->SetWorldLocation(NewLocation);
}

void ACorkboardDiegetic::FinishDiscoverTimeline()
{
	FVector Vector = CameraComponent->GetComponentLocation() - LocationDL;
	float XPos = FVector::DotProduct(Vector, PlaneAxisX) / PlaneWidth;
	float YPos = FVector::DotProduct(Vector, PlaneAxisY) / PlaneHeight;
	

	CurrentPosition = FVector2D(FMath::Clamp(XPos,0.0f,1.0f),FMath::Clamp(YPos,0.0f,1.0f));
	
	if (CluesUpdated.IsEmpty())
	{
		Lock(false);
		return;
	}
	StartDiscoverTimeline();
	
}

