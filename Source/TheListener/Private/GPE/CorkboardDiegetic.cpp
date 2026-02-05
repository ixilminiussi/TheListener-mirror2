// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "GPE/CorkboardDiegetic.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"


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
}

// Called when the game starts or when spawned
void ACorkboardDiegetic::BeginPlay()
{
	Super::BeginPlay();
	
	double XLimit = CameraPlaneMeshComponent->Bounds.BoxExtent.X;
	MoveRelativeLimitX = FVector2D(-XLimit,XLimit);
	double YLimit = CameraPlaneMeshComponent->Bounds.BoxExtent.Y;
	MoveRelativeLimitY = FVector2D(-YLimit,YLimit);
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

	//Camera Zoom Part
	CurrentZoomScale += LastZoomInputValue*(DeltaSeconds*ZoomScaleSpeed);
	CurrentZoomScale = FMath::Clamp(CurrentZoomScale, 0.f, 1.f);
	float CurrentFOV = FOVCurve->GetFloatValue(CurrentZoomScale);
	CameraComponent.Get()->SetFieldOfView(CurrentFOV);

	//Camera Move Part
	FVector CamRelativeLocation = CameraComponent.Get()->GetRelativeLocation();
	float CurrentMoveSpeed = CamSpeedCurve->GetFloatValue(CurrentZoomScale);
	FVector2D MoveValue = LastMoveInputValue*DeltaSeconds*CurrentMoveSpeed;
	
	CamRelativeLocation += FVector(0, MoveValue.X, MoveValue.Y);
	CamRelativeLocation = FVector(CamRelativeLocation.X,
		FMath::Clamp(CamRelativeLocation.Y,MoveRelativeLimitX.X,MoveRelativeLimitX.Y),
		FMath::Clamp(CamRelativeLocation.Z,MoveRelativeLimitY.X,MoveRelativeLimitY.Y));
	CameraComponent.Get()->SetRelativeLocation(CamRelativeLocation);
}

void ACorkboardDiegetic::UpdateClue(class UClueAsset* Clue)
{
}

void ACorkboardDiegetic::MoveInput(const struct FInputActionValue& Value)
{
	LastMoveInputValue = Value.Get<FVector2D>();
}

void ACorkboardDiegetic::ZoomInput(const struct FInputActionValue& Value)
{
	LastZoomInputValue = Value.Get<float>();
}

