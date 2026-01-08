#include "Player/Components/HandComponent.h"

#include "Camera/CameraComponent.h"
#include "Player/LukaCharacter.h"
#include "GPE/Obji.h"

UHandComponent::UHandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHandComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* ParentActor = GetAttachParentActor();
	HeadCamera = Cast<UCameraComponent>(ParentActor->GetComponentByClass(UCameraComponent::StaticClass()));
	LukaParent = Cast<ALukaCharacter>(ParentActor);

	check(LukaParent);
	check(HeadCamera);
}

void UHandComponent::UpdatePreviewLocation()
{
	check(HeldObji);
	check(IsValid(ObjiPreview));
	check(HeadCamera);
	check(LukaParent)

	const FVector StartLocation = HeadCamera->GetComponentLocation();
	const FVector EndLocation = StartLocation + HeadCamera->GetForwardVector() * LukaParent->GetInteractiveMaxRange();
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(LukaParent);
	QueryParams.AddIgnoredActor(ObjiPreview);
	QueryParams.AddIgnoredActor(HeldObji);

	FDropZoneInfo DropZoneInfo{};
	// must check with held Obji, he remembers said drop zone for when you ask to drop it
	HeldObji->CheckAcceptableDropZone(DropZoneInfo, StartLocation, EndLocation, QueryParams);
	if (DropZoneInfo.IsValid())
	{
		bObjiPreviewIsValid = true;
		ObjiPreview->SetActorLocation(DropZoneInfo.Location);
		ObjiPreview->SetActorRotation(DropZoneInfo.Rotation);
	}
	else
	{
		bObjiPreviewIsValid = false;
		ObjiPreview->SetActorLocation(FVector::UnitZ() * 10000.f);
	}
}


void UHandComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (HasObji())
	{
		UpdateObjiPosition();
		UpdatePreviewLocation();
	}
}

FVector UHandComponent::GetTrueObjiLocation() const
{
	if (!HeldObji)
	{
		return GetComponentLocation();
	}
	FVector EndLocation = GetComponentLocation();
	const FRotator EndRotation = HeldObji->GetActorRotation();
	FVector BoundsOrigin;
	FVector BoundsExtent;

	HeldObji->SetActorRotation(FRotator{0.f, 0.f, 0.f});
	HeldObji->GetActorBounds(true, BoundsOrigin, BoundsExtent);
	HeldObji->SetActorRotation(EndRotation);

	if (BoundsExtent.X > LeftAnchor)
	{
		EndLocation += GetRightVector() * -(BoundsExtent.X - LeftAnchor);
	}
	if (BoundsExtent.Y > BottomAnchor)
	{
		EndLocation += GetUpVector() * (BoundsExtent.Z - BottomAnchor);
	}
	if (BoundsExtent.X > RightAnchor)
	{
		EndLocation += GetRightVector() * (BoundsExtent.X - RightAnchor);
	}
	if (BoundsExtent.Y > TopAnchor)
	{
		EndLocation += GetUpVector() * -(BoundsExtent.Z - TopAnchor);
	}

	return EndLocation;
}

void UHandComponent::UpdateObjiPosition() const
{
	check(HeldObji);

	if (!HeldObji->IsMoving())
	{
		HeldObji->SetActorLocation(GetTrueObjiLocation());
	}

	if (!HeldObji->IsRotating())
	{
		const FRotator EndRotation = GetComponentRotation();
		HeldObji->SetActorRotation(EndRotation);
	}
}

AObji* UHandComponent::GetHeldObji() const
{
	return HeldObji;
}

void UHandComponent::PickupObji(AObji* Obji)
{
	check(Obji);
	ObjiPreview = Obji->CreatePreview();
	HeldObji = Obji;
}

AObji* UHandComponent::ReleaseObji()
{
	check(HeldObji);

	if (HeldObji->Drop())
	{
		AObji* Ptr = HeldObji;
		HeldObji = nullptr;

		ObjiPreview->Destroy();
		ObjiPreview = nullptr;
		return Ptr;
	}

	return nullptr;
}

void UHandComponent::PrepareTraceParams(FCollisionQueryParams& QueryParams) const
{
	if (HeldObji)
	{
		QueryParams.AddIgnoredActor(HeldObji);
	}
	if (ObjiPreview)
	{
		QueryParams.AddIgnoredActor(ObjiPreview);
	}
}