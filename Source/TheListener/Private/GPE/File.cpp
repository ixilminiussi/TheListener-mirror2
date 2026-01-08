#include "GPE/File.h"

#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Player/LukaController.h"
#include "GPE/Toy.h"
#include "UI/InspectWidget.h"
#include "AkComponent.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "UI/LukaHUD.h"
#include "UI/PlayWidget.h"

AFile::AFile(const FObjectInitializer& ObjectInitializer) : Super{ObjectInitializer}
{
	PrimaryActorTick.bCanEverTick = true;

	check(RootComponent);
	SoundPosition = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, "SoundPosition");
	SoundPosition->SetupAttachment(RootComponent);
	check(SoundPosition);

	AkComponent = ObjectInitializer.CreateDefaultSubobject<UAkComponent>(this, "AkComponent");
	AkComponent->SetupAttachment(SoundPosition);
	AkComponent->SetRelativeLocation(FVector::ZeroVector);
	check(AkComponent);
}

void AFile::BeginPlay()
{
	Super::BeginPlay();
}

UInspectWidget* AFile::GetInspectWidget() 
{
	if (InspectWidget == nullptr)
	{
		//Man you joking, right ? You forgot to assign a class of widget to create for the file
		ensureAlways(InspectWidgetClass);
		InspectWidget = CreateWidget<UInspectWidget>(GetWorld(), InspectWidgetClass);
		APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ALukaHUD* HUD = Cast<ALukaHUD>(Controller->GetHUD());
		UCanvasPanel* Panel = HUD->GetInspectablePanel();
		Panel->AddChild(InspectWidget);
		InspectWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	return InspectWidget;
}

void AFile::ShowInspectable() 
{
	//If no widget force create one
	if (InspectWidget == nullptr) {GetInspectWidget();}
	InspectWidget->SetVisibility(ESlateVisibility::Visible);
}

void AFile::HideInspectable() 
{
	InspectWidget->SetVisibility(ESlateVisibility::Collapsed);
}

bool AFile::IsInspectableVisible()
{
	return InspectWidget->GetVisibility() == ESlateVisibility::Visible;
}

void AFile::CustomCheck(FDropZoneInfo& DropZoneInfo, const FVector& RayCheckBegin, const FVector& RayCheckEnd,
                        FCollisionQueryParams QueryParams)
{
	check(LukaController); // remember to call this function on the HELD Obji
	check(StaticMeshComponent);

	FHitResult HitResult;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, RayCheckBegin, RayCheckEnd, ECC_WorldStatic,
	                                                       QueryParams);

	float UpAlignment = FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector);

	// Cannot place on Toy or Objis EXCEPT for OTHER Files
	AToy* Toy = Cast<AToy>(HitResult.GetActor());
	AObji* Obji = Cast<AObji>(HitResult.GetActor());
	AFile* File = Cast<AFile>(HitResult.GetActor());
	if (!bHit || Toy != nullptr || (Obji != nullptr && File == nullptr)
		|| (UpAlignment < 0.9f && FMath::Abs(UpAlignment) > 0.1f))
	{
		DropZoneInfo.Fail();
		LastDropZoneInfo = DropZoneInfo;
		return;
	}

	UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh();
	check(StaticMesh);
	const FBoxSphereBounds& Bounds = StaticMesh->GetBoundingBox();
	const float HalfHeight = Bounds.BoxExtent.X - Bounds.Origin.X;
	DropZoneInfo.Location = HitResult.Location + FVector(0.f, 0.f, HalfHeight);

	// on the floor
	if (UpAlignment > 0.99f)
	{
		const FVector Normal = HitResult.ImpactNormal;
		const FVector ViewVector = (RayCheckEnd - RayCheckBegin).GetSafeNormal();
		const FVector Tangent = FVector::CrossProduct(Normal, ViewVector);

		FVector Z = Tangent.GetSafeNormal();
		FVector X = UpAlignment > 0.99f ? -Normal.GetSafeNormal() : Normal.GetSafeNormal();
		FVector Y = FVector::CrossProduct(Z, X).GetSafeNormal();

		const FQuat RotationQuat = FQuat(X, FMath::DegreesToRadians(90.0f - AdditiveRotation));

		Y = RotationQuat.RotateVector(Y);
		Z = RotationQuat.RotateVector(Z);

		FMatrix RotationMatrix(X, Y, Z, FVector::ZeroVector);

		DropZoneInfo.Rotation = RotationMatrix.Rotator();
	}
	else
	{
		// on the wall
		const FVector Normal = -HitResult.ImpactNormal;
		const FQuat RotationQuat = FQuat(Normal, FMath::DegreesToRadians(-AdditiveRotation));

		DropZoneInfo.Rotation = (RotationQuat * Normal.ToOrientationQuat()).Rotator();
	}

	DropZoneInfo.Succeed();
	LastDropZoneInfo = DropZoneInfo;
}


void AFile::OnDrop()
{
	Super::OnDrop();

	check(StaticMeshComponent);
	StaticMeshComponent->SetEnableGravity(false);
	StaticMeshComponent->SetSimulatePhysics(false);
}
