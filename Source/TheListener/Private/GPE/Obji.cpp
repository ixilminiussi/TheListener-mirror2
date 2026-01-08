#include "GPE/Obji.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EventSubsystem.h"
#include "Player/Components/HandComponent.h"
#include "Player/LukaCharacter.h"
#include "Components/TimelineComponent.h"
#include "Player/LukaController.h"
#include "Curves/CurveFloat.h"
#include "GPE/File.h"
#include "GPE/Toy.h"
#include "Kismet/KismetMathLibrary.h"
#include "System/Events/EventCondition.h"

// Sets default values
AObji::AObji(const FObjectInitializer& ObjectInitializer) : Super{ObjectInitializer}
{
	CreateTimelines();
}

void AObji::CheckAcceptableDropZone(FDropZoneInfo& DropZoneInfo, const FVector& RayCheckBegin,
                                    const FVector& RayCheckEnd, FCollisionQueryParams QueryParams)
{
	CustomCheck(DropZoneInfo, RayCheckBegin, RayCheckEnd, QueryParams);
	LastDropZoneInfo = DropZoneInfo;
}

void AObji::CustomCheck(FDropZoneInfo& DropZoneInfo, const FVector& RayCheckBegin, const FVector& RayCheckEnd,
                        FCollisionQueryParams QueryParams)
{
	check(LukaController); // remember to call this function on the HELD Obji
	check(StaticMeshComponent);

	FHitResult HitResult;

	QueryParams.AddIgnoredActor(this);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, RayCheckBegin, RayCheckEnd, ECC_WorldStatic,
	                                                       QueryParams);

	// Cannot place on Toy or Objis EXCEPT for files
	AToy* Toy = Cast<AToy>(HitResult.GetActor());
	AObji* Obji = Cast<AObji>(HitResult.GetActor());
	AFile* File = Cast<AFile>(HitResult.GetActor());
	if (!bHit || Toy != nullptr || (Obji != nullptr && File == nullptr)
		|| (FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector) < 0.9f))
	{
		DropZoneInfo.Fail();
		return;
	}

	const FBoxSphereBounds& Bounds = StaticMeshComponent->GetStaticMesh()->GetBounds();
	const float HalfHeight = Bounds.BoxExtent.Z - Bounds.Origin.Z;
	DropZoneInfo.Location = HitResult.Location + FVector(0.f, 0.f, HalfHeight);

	const FVector Normal = HitResult.ImpactNormal;
	const FVector ViewVector = (RayCheckEnd - RayCheckBegin).GetSafeNormal();
	const FVector Tangent = FVector::CrossProduct(Normal, ViewVector);

	FVector Z = Tangent.GetSafeNormal();
	FVector X = Normal.GetSafeNormal();
	FVector Y = FVector::CrossProduct(Z, X).GetSafeNormal();

	const FQuat RotationQuatY = FQuat(Y, FMath::DegreesToRadians(90.0f));

	X = RotationQuatY.RotateVector(X);
	Z = RotationQuatY.RotateVector(Z);

	const FQuat RotationQuatZ = FQuat(Z, FMath::DegreesToRadians(90.0f + AdditiveRotation));

	Y = RotationQuatZ.RotateVector(Y);
	X = RotationQuatZ.RotateVector(X);

	FMatrix RotationMatrix(X, Y, Z, FVector::ZeroVector);

	DropZoneInfo.Rotation = RotationMatrix.Rotator();

	// if (!CheckValidity(DropZoneInfo.Location, DropZoneInfo.Rotation.Quaternion()))
	// {
	// 	DropZoneInfo.Fail();
	// 	LastDropZoneInfo = DropZoneInfo;
	// 	return;
	// }

	DropZoneInfo.Succeed();
}

AObji* AObji::CreatePreview() const
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Template = nullptr;
	SpawnParams.Instigator = GetInstigator();

	AObji* Preview = GetWorld()->SpawnActor<AObji>(
		StaticClass(),
		GetActorLocation(),
		FRotator::ZeroRotator,
		SpawnParams
	);

	UStaticMeshComponent* PreviewStaticMeshComponent = Preview->GetStaticMeshComponent();

	if (ensure(StaticMeshComponent) && ensure(PreviewStaticMeshComponent))
	{
		PreviewStaticMeshComponent->SetStaticMesh(StaticMeshComponent->GetStaticMesh());
		PreviewStaticMeshComponent->SetWorldScale3D(StaticMeshComponent->GetComponentScale());
		PreviewStaticMeshComponent->SetSimulatePhysics(false);
		PreviewStaticMeshComponent->SetCollisionProfileName(FName("NoCollision"));
		for (int i = 0; i < PreviewStaticMeshComponent->GetNumMaterials(); i++)
		{
			if (!ensure(PreviewMaterial))
			{
				PreviewStaticMeshComponent->SetMaterial(i, StaticMeshComponent->GetMaterial(i));
			}
			else
			{
				PreviewStaticMeshComponent->SetMaterial(i, PreviewMaterial);
			}
		}
	}

	return Preview;
}

bool AObji::CheckValidity(const FVector& Location, const FQuat& QuatRotation) const
{
	check(StaticMeshComponent);

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(this);

	const bool bIsOverlapping = GetWorld()->OverlapAnyTestByChannel(Location, QuatRotation, ECC_WorldStatic,
	                                                                StaticMeshComponent->GetCollisionShape(),
	                                                                QueryParams);

	return !bIsOverlapping;
}

TArray<TSubclassOf<AInteractable>> AObji::GetInteractablesAllowed() const
{
	return AllowedInteractables;
}

TArray<TSubclassOf<AToy>> AObji::GetToysAllowed() const
{
	return AllowedToys;
}

void AObji::Interact(const ALukaCharacter* Luka)
{
	Super::Interact(Luka);

	if (auto HandComponent = Luka->GetHandComponent())
	{
		check(StaticMeshComponent);
		StaticMeshComponent->SetSimulatePhysics(false);
		StaticMeshComponent->SetEnableGravity(false);
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		check(HandComponent);
		MoveTo(HandComponent);
		RotateTo(HandComponent);

		SetHoverWidgetVisibility(false);
		SetActiveWidgetVisibility(true);
	}
}

bool AObji::Drop()
{
	if (!LastDropZoneInfo.IsValid())
	{
		LastDropZoneInfo.Fail(); // reset
		return false;
	}

	FTimeline* MoveTimeline = MoveTo(LastDropZoneInfo.Location);
	FTimeline* RotateTimeline = RotateTo(LastDropZoneInfo.Rotation);

	const auto LambdaFunc = FOnTimelineEventStatic::CreateLambda([this]
	{
		OnDrop();
	});
	if (MoveTimeline->GetTimelineLength() > RotateTimeline->GetTimelineLength())
	{
		MoveTimeline->SetTimelineFinishedFunc(LambdaFunc);
	}
	else
	{
		RotateTimeline->SetTimelineFinishedFunc(LambdaFunc);
	}

	SetActiveWidgetVisibility(false);
	return true;
}

void AObji::SetupInput(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (InputActionPreviewRotation)
	{
		EnhancedInputComponent->BindAction(InputActionPreviewRotation, ETriggerEvent::Triggered, this,
		                                   &AObji::RotatePreview);
	}
}

void AObji::OnInteract(AController* NewController)
{
	Super::OnInteract(NewController);

	if (!ensure(InputMappingContext))
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LukaController->GetLocalPlayer()->GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>(LukaController->GetLocalPlayer());
	check(InputSubsystem);
	InputSubsystem->AddMappingContext(InputMappingContext, 1);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(LukaController->InputComponent))
	{
		SetupInput(EnhancedInputComponent); // TODO: Move to Begin Play
	}

	if (UEventSubsystem* EventSubsystem = GetWorld()->GetSubsystem<UEventSubsystem>(); ensure(EventSubsystem))
	{
		const FConditionKey Key = UObjiHeldCondition::GenerateKey(GetClass());
		EventSubsystem->SetConditionValue(Key, true, true);
	}

	OnPickup_();
	
}

void AObji::OnDrop()
{
	check(StaticMeshComponent);
	StaticMeshComponent->SetEnableGravity(true);
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	if (!InputMappingContext)
	{
		return;
	}

	if (LukaController != nullptr)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LukaController->GetLocalPlayer()->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(LukaController->GetLocalPlayer());
		InputSubsystem->RemoveMappingContext(InputMappingContext);
		
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(LukaController->InputComponent))
		{
			EnhancedInputComponent->ClearBindingsForObject(this);
		}
	}

	OnDrop_();
}

void AObji::BeginPlay()
{
	Super::BeginPlay();

	if (!RotateBlendTimeline || !MoveBlendTimeline)
	{
		CreateTimelines();
	}
}

void AObji::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (MoveBlendTimeline)
	{
		delete MoveBlendTimeline;
		MoveBlendTimeline = nullptr;
	}
	if (RotateBlendTimeline)
	{
		delete RotateBlendTimeline;
		RotateBlendTimeline = nullptr;
	}
}

struct FTimeline* AObji::MoveTo(const UHandComponent* TargetComponent, bool bTeleport)
{
	check(StaticMeshComponent);
	StaticMeshComponent->SetSimulatePhysics(false);

	bMoving = true;

	check(TargetComponent);

	if (bTeleport)
	{
		SetActorRelativeLocation(TargetComponent->GetComponentLocation());
		return nullptr;
	}

	if (!ensure(MoveBlendTimeline))
	{
		CreateTimelines();
	}
	
	BeginLocation = GetRootComponent()->GetRelativeLocation();
	TargetLocationComponent = TargetComponent;

	MoveBlendTimeline->PlayFromStart();
	GetWorldTimerManager().SetTimer(MoveBlendTimer, this, &AObji::TickMoveBlendTimeline, 0.001f, true, 0.0f);

	MoveBlendTimeline->SetTimelineFinishedFunc(FOnTimelineEventStatic::CreateLambda([this] { bMoving = false; }));

	return MoveBlendTimeline;
}

FTimeline* AObji::RotateTo(const UHandComponent* TargetComponent, bool bTeleport)
{
	check(StaticMeshComponent);
	StaticMeshComponent->SetSimulatePhysics(false);

	bRotating = true;

	check(TargetComponent);

	if (bTeleport)
	{
		SetActorRelativeRotation(TargetComponent->GetComponentRotation());
		return nullptr;
	}

	if (!ensure(RotateBlendTimeline)) {
		CreateTimelines();
	}

	BeginRotation = GetRootComponent()->GetRelativeRotation();
	TargetRotationComponent = TargetComponent;

	RotateBlendTimeline->PlayFromStart();
	GetWorldTimerManager().SetTimer(RotateBlendTimer, this, &AObji::TickRotateBlendTimeline, 0.001f, true, 0.0f);

	RotateBlendTimeline->SetTimelineFinishedFunc(FOnTimelineEventStatic::CreateLambda([this] { bRotating = false; }));

	return RotateBlendTimeline;
}

void AObji::RotatePreview(const struct FInputActionValue& Value)
{
	AdditiveRotation += Value.Get<float>() * RotationStrength;
}

FTimeline* AObji::MoveTo(const FVector& Location, bool bTeleport)
{
	check(StaticMeshComponent);
	StaticMeshComponent->SetSimulatePhysics(false);

	bMoving = true;
	TargetLocationComponent = nullptr;

	if (bTeleport)
	{
		SetActorRelativeLocation(Location);
		return nullptr;
	}

	if (!ensure(MoveBlendTimeline)) {
		CreateTimelines();
	}
	BeginLocation = GetRootComponent()->GetRelativeLocation();
	TargetLocation = Location;

	MoveBlendTimeline->PlayFromStart();
	GetWorldTimerManager().SetTimer(MoveBlendTimer, this, &AObji::TickMoveBlendTimeline, 0.001f, true, 0.0f);

	MoveBlendTimeline->SetTimelineFinishedFunc(FOnTimelineEventStatic::CreateLambda([this] { bMoving = false; }));

	return MoveBlendTimeline;
}

FTimeline* AObji::RotateTo(const FRotator& Rotation, bool bTeleport)
{
	check(StaticMeshComponent);
	StaticMeshComponent->SetSimulatePhysics(false);

	bRotating = true;
	TargetRotationComponent = nullptr;

	if (bTeleport)
	{
		SetActorRelativeRotation(Rotation);
		return nullptr;
	}

	if (!ensure(RotateBlendTimeline)) {
		CreateTimelines();
	}

	BeginRotation = GetRootComponent()->GetRelativeRotation();
	TargetRotation = Rotation;

	RotateBlendTimeline->PlayFromStart();
	GetWorldTimerManager().SetTimer(RotateBlendTimer, this, &AObji::TickRotateBlendTimeline, 0.001f, true, 0.0f);

	RotateBlendTimeline->SetTimelineFinishedFunc(FOnTimelineEventStatic::CreateLambda([this] { bRotating = false; }));

	return RotateBlendTimeline;
}

void AObji::TickMoveBlendTimeline()
{
	if (!ensure(MoveBlendTimeline)) {
		CreateTimelines();
	}

	if (MoveBlendTimeline->IsPlaying())
	{
		MoveBlendTimeline->TickTimeline(0.001f);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(MoveBlendTimer);
		SetLifeSpan(0);
	}
}

void AObji::TickRotateBlendTimeline()
{
	if (!ensure(RotateBlendTimeline)) {
		CreateTimelines();
	}

	if (RotateBlendTimeline->IsPlaying())
	{
		RotateBlendTimeline->TickTimeline(0.001f);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(RotateBlendTimer);
		SetLifeSpan(0);
	}
}

void AObji::EffectMoveBlend(const float Value)
{
	if (TargetLocationComponent)
	{
		SetActorLocation(UKismetMathLibrary::VLerp(BeginLocation, TargetLocationComponent->GetTrueObjiLocation(),
		                                           Value));
	}
	else
	{
		SetActorRelativeLocation(UKismetMathLibrary::VLerp(BeginLocation, TargetLocation, Value));
	}
}

void AObji::EffectRotateBlend(const float Value)
{
	if (TargetRotationComponent)
	{
		SetActorRotation(UKismetMathLibrary::RLerp(BeginRotation, TargetRotationComponent->GetComponentRotation(),
		                                           Value, true));
	}
	else
	{
		SetActorRelativeRotation(UKismetMathLibrary::RLerp(BeginRotation, TargetRotation, Value, true));
	}
}

void AObji::CreateTimelines()
{
	if (MoveBlendCurve)
	{
		MoveBlendTimeline = new FTimeline{};
		FOnTimelineFloat ProgressFunction{};
		ProgressFunction.BindUFunction(this, "EffectMoveBlend");
		MoveBlendTimeline->AddInterpFloat(MoveBlendCurve, ProgressFunction, FName{TEXT("EffectLerp")});
	}

	if (RotateBlendCurve)
	{
		RotateBlendTimeline = new FTimeline{};
		FOnTimelineFloat ProgressFunctionRotate{};
		ProgressFunctionRotate.BindUFunction(this, "EffectRotateBlend");
		RotateBlendTimeline->AddInterpFloat(RotateBlendCurve, ProgressFunctionRotate, FName{TEXT("EffectLerp")});
	}
}
