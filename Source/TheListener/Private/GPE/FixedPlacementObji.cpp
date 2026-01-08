// Fill out your copyright notice in the Description page of Project Settings.


#include "GPE/FixedPlacementObji.h"

#include "Components/SphereComponent.h"

void AFixedPlacementObji::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (TObjectPtr<AActor>& DropLocation : DropLocations)
	{
		DropLocation->Destroy();
	}

	Super::EndPlay(EndPlayReason);
}

void AFixedPlacementObji::CustomCheck(FDropZoneInfo& DropZoneInfo, const FVector& RayCheckBegin,
                                      const FVector& RayCheckEnd, FCollisionQueryParams QueryParams)
{
	for (const AActor* DropTransform : DropLocations)
	{
		if (FHitResult HitResult; DropTransform->GetComponentByClass<USphereComponent>()->LineTraceComponent(
			HitResult, RayCheckBegin, RayCheckEnd, QueryParams))
		{
			DropZoneInfo.Location = DropTransform->GetActorLocation();
			DropZoneInfo.Rotation = DropTransform->GetActorRotation();
			SetActiveWidgetVisibility(true);
			DropZoneInfo.Succeed();
			
			return;
		}
	}
	SetActiveWidgetVisibility(false);
	DropZoneInfo.Fail();
}

void AFixedPlacementObji::OnInteract(class AController* NewController)
{
	Super::OnInteract(NewController);
}

void AFixedPlacementObji::OnDrop()
{
	Super::OnDrop();

	check(StaticMeshComponent);
	StaticMeshComponent->SetEnableGravity(false);
	StaticMeshComponent->SetSimulatePhysics(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFixedPlacementObji::RotatePreview(const FInputActionValue& Value)
{
	// TODO: remove Rotation prompts
}

void AFixedPlacementObji::AddDropLocation(const FTransform NewDropLocation, const float Radius)
{
	FActorSpawnParameters Params;
	Params.Owner = this;

	if (AActor* DropActor = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), NewDropLocation, Params);
		ensure(DropActor))
	{
		USphereComponent* SphereComponent = NewObject<USphereComponent>(DropActor);
		SphereComponent->InitSphereRadius(Radius);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereComponent->RegisterComponent();
		DropActor->SetRootComponent(SphereComponent);

		SphereComponent->SetRelativeLocation(FVector(0, 0, 0));
		SphereComponent->SetRelativeRotation(FRotator(0, 0, 0));

		DropActor->SetActorTransform(NewDropLocation);
		DropLocations.Add(DropActor);
	}
}
