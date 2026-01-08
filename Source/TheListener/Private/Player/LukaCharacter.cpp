#include "Player/LukaCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Player/Components/HandComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GPE/Interactable.h"
#include "GPE/Obji.h"
#include "GPE/Toy.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LukaController.h"
#include "Structs/PlayerData.h"
#include "System/Core/BaseGameInstance.h"
#include "UI/Menus/SettingsDataAsset.h"
#include "UI/Menus/SettingsSave.h"

ALukaCharacter::ALukaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	HandComponent = CreateDefaultSubobject<UHandComponent>("Hand");
	check(RootComponent);
	check(SpringArmComponent);
	check(CameraComponent);
	check(HandComponent);

	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = true;

	HandComponent->SetupAttachment(CameraComponent);
}

void ALukaCharacter::BeginPlay()
{
	Super::BeginPlay();


	if (PlayerData)
	{
		MapPlayerData(PlayerData);
	}

	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);
	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);
	SettingsSave->OnSettingsChanged.AddDynamic(this, &ALukaCharacter::MapSettingsData);
	MapSettingsData();
}

void ALukaCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	InteractiveRayCheck();
}

void ALukaCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!ensure(InputMappingContext))
	{
		UE_LOG(LogTemp, Error, TEXT("Missing Luka InputMappingContext"));
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
	{
		EnableInput(PlayerController);

		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>();
		InputSubsystem->AddMappingContext(InputMappingContext, 1);

		if (ALukaController* LukaController = Cast<ALukaController>(NewController))
		{
			if (PlayerData)
			{
				LukaController->MapPlayerData(PlayerData);
			}
		}
	}
	if (AToy* CurrentToyInView = GetToyInView()) { CurrentToyInView->SetHoverWidgetVisibility(true); }
}

void ALukaCharacter::UnPossessed()
{
	const APlayerController* PlayerController = Cast<APlayerController>(GetController());

	Super::UnPossessed();

	if (!InputMappingContext || !ensure(PlayerController))
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>())
	{
		InputSubsystem->RemoveMappingContext(InputMappingContext);
	}
}

void ALukaCharacter::MapPlayerData(const UPlayerData* InPlayerData)
{
	check(InPlayerData);

	// Camera Begin
	check(CameraComponent);
	CameraComponent->SetFieldOfView(InPlayerData->FOV);
	CameraComponent->SetOverscan(InPlayerData->Overscan);

	check(SpringArmComponent);
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 0.f, InPlayerData->CamHeight));

	HorizontalSensitivity = InPlayerData->HorizontalSensitivity;
	VerticalSensitivity = InPlayerData->VerticalSensitivity;
	// Camera End

	// Movement Begin
	GetCharacterMovement()->MaxWalkSpeed = InPlayerData->MaxWalkSpeed;
	LateralMaxWalkSpeed = InPlayerData->LateralMaxWalkSpeed;
	BackwardMaxWalkSpeed = InPlayerData->BackwardMaxWalkSpeed;
	GetCharacterMovement()->MaxAcceleration = InPlayerData->MaxAcceleration;
	GetCharacterMovement()->BrakingDecelerationWalking = InPlayerData->BrakingDecelerationWalking;
	GetCharacterMovement()->MaxStepHeight = InPlayerData->MaxStepHeight;
	GetCharacterMovement()->GroundFriction = InPlayerData->GroundFriction;
	GetCharacterMovement()->SetWalkableFloorAngle(InPlayerData->WalkableFloorAngle);
	// Movement End

	// Interaction Begin
	InteractionMaxRange = InPlayerData->InteractionRange;
	// Interaction End

	// Feedbacks Begin
	MinFootstepDistance = InPlayerData->MinFootstepDistance;
	MaxFootstepDistance = InPlayerData->MaxFootstepDistance;
	MaxFootstepTime = InPlayerData->MaxFootstepTime;
	// Feedback End
}

class UPlayerData* ALukaCharacter::GetPlayerData() const
{
	return PlayerData;
}

void ALukaCharacter::MapSettingsData()
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	HorizontalSensitivity = SettingsSave->GetHorizontalSensitivity();
	VerticalSensitivity = SettingsSave->GetVerticalSensitivity();
}

UInputMappingContext* ALukaCharacter::GetLukaMappingContext() const
{
	return InputMappingContext;
}

void ALukaCharacter::Look(const FInputActionValue& Value)
{
	AddControllerYawInput(Value.Get<FVector2d>().X * HorizontalSensitivity);
	AddControllerPitchInput(-Value.Get<FVector2d>().Y * VerticalSensitivity);
	OnLook.Broadcast();
}

void ALukaCharacter::Move(const FInputActionValue& Value)
{
	static float TimeOfLastFootstep = GetWorld()->GetTimeSeconds();
	static FVector LocationOfLastFootstep = GetActorLocation();

	const FVector2d FinalValue = Value.Get<FVector2d>();
	AddMovementInput(GetActorForwardVector(), FinalValue.Y > 0
		                                          ? FinalValue.Y
		                                          : FinalValue.Y * BackwardMaxWalkSpeed / GetCharacterMovement()->
		                                          GetMaxSpeed());
	AddMovementInput(GetActorRightVector(), FinalValue.X * LateralMaxWalkSpeed / GetCharacterMovement()->GetMaxSpeed());

	const float FootstepDistance = FVector::Dist(GetActorLocation(), LocationOfLastFootstep);
	if (FootstepDistance >= MaxFootstepDistance)
	{
		OnStep(1.0);
		TimeOfLastFootstep = GetWorld()->GetTimeSeconds();
		LocationOfLastFootstep = GetActorLocation();
	}
	if (GetWorld()->GetTimeSeconds() - TimeOfLastFootstep >= MaxFootstepTime && FootstepDistance >= MinFootstepDistance)
	{
		OnStep(FootstepDistance / (MaxFootstepDistance - MinFootstepDistance));
		TimeOfLastFootstep = GetWorld()->GetTimeSeconds();
		LocationOfLastFootstep = GetActorLocation();
	}

	float VelocityMagnitude = GetCharacterMovement()->GetLastUpdateVelocity().Length();
	VelocityMagnitude /= GetCharacterMovement()->GetMaxSpeed();
	OnMove.Broadcast(VelocityMagnitude);
}

void ALukaCharacter::HandleStartMoving(const FInputActionValue& Value) 
{
	OnStartMoving.Broadcast();
}

void ALukaCharacter::HandleStopMoving(const FInputActionValue& Value) 
{
	OnStopMoving.Broadcast();
}

TObjectPtr<class UHandComponent> ALukaCharacter::GetHandComponent() const
{
	return HandComponent;
}

bool ALukaCharacter::HasObji() const
{
	check(HandComponent);
	return HandComponent->HasObji();
}

AObji* ALukaCharacter::GetHeldObji() const
{
	check(HandComponent);
	return HandComponent->GetHeldObji();
}

void ALukaCharacter::InteractiveRayCheck()
{
	
	if (HasObji() && HandComponent->GetHeldObji()->GetInteractablesAllowed().Num() == 0 )
	{
		return;
	}
	
	
	AToy* PreviousToyInView = ToyInView;
	AInteractable* PreviousInteractableInView = InteractableInView;
	InteractableInView = nullptr;
	ToyInView = nullptr;

	// Check if hitting Hitbox == BEGIN =====================
	TArray<FHitResult> HitResults;
	const FVector StartLocation = CameraComponent->GetComponentLocation();
	const FVector EndLocation = StartLocation + CameraComponent->GetForwardVector() * InteractionMaxRange;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(InteractiveCollisionChannel);

	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(LineTrace), true, this);

	check(HandComponent);
	HandComponent->PrepareTraceParams(TraceParams);

	bool bHit = GetWorld()->LineTraceMultiByObjectType(HitResults, StartLocation, EndLocation, ObjectQueryParams,
	                                                   TraceParams);
	// Check if hitting Hitbox == END =======================

	// Check if hitting Walls == BEGIN ======================
	FHitResult WallHitResult;
	const bool bWallHit = GetWorld()->LineTraceSingleByChannel(WallHitResult, StartLocation, EndLocation,
	                                                           ECC_WorldStatic,
	                                                           TraceParams);

	const float WallDistance = bWallHit ? WallHitResult.Distance : TNumericLimits<float>::Max();
	// Check if hitting Walls == END ========================

	// Check if we already had an object in view to avoid calling unnecessary "OnEnter" events
	const bool bHadInteractableInView = PreviousInteractableInView != nullptr;
	const bool bHadToyInView = PreviousToyInView != nullptr;

	float ClosestToyDistance = TNumericLimits<float>::Max();
	float ClosestInteractableDistance = TNumericLimits<float>::Max();

	for (const FHitResult& HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();

		if (HitResult.Distance > WallDistance)
		{
			UE_LOG(LogTemp, Warning, TEXT("%f, %f"), HitResult.Distance, WallDistance);
			continue;
		}
		if (AInteractable* NewInteractableInView = Cast<AInteractable>(HitActor))
		{
			if (HitResult.Distance < ClosestInteractableDistance && NewInteractableInView->IsEnabled())
			{
				ClosestInteractableDistance = HitResult.Distance;
				InteractableInView = NewInteractableInView;
				if (ToyInView)
				{
					ToyInView = nullptr;
				}
			}
		}

		if (AToy* NewToyInView = Cast<AToy>(HitActor))
		{
			if (HitResult.Distance < ClosestToyDistance && NewToyInView->IsEnabled())
			{
				ClosestToyDistance = HitResult.Distance;
				ToyInView = NewToyInView;
			}
		}
	}

	const bool bHasInteractableInView = InteractableInView != nullptr;
	const bool bHasToyInView = ToyInView != nullptr;

	if (HasObji())
	{
		TArray<TSubclassOf<AInteractable>> InteractablesAllowed =  HandComponent->GetHeldObji()->GetInteractablesAllowed();
		TArray<TSubclassOf<AToy>> ToysAllowed =  HandComponent->GetHeldObji()->GetToysAllowed();
		if (bHasInteractableInView){
			if (!InteractablesAllowed.Contains(InteractableInView->GetClass()))
			{
				InteractableInView = nullptr;
				return;
			}
		}
		else if (bHasToyInView) {
			if (!ToysAllowed.Contains(ToyInView->GetClass()))
			{
				ToyInView = nullptr;
				return;
			}
		}
	}

	if ((!bHadInteractableInView && bHasInteractableInView)
		|| (!bHadToyInView && bHasToyInView)
		|| (bHasToyInView && !bHasInteractableInView && bHadInteractableInView))
	{
		if (bHasInteractableInView)
		{
			check(InteractableInView);
			OnBeginAnyInView.Broadcast();
		}
		else if (bHasToyInView)
		{
			check(ToyInView);
			OnBeginAnyInView.Broadcast();
		}
	}
	if (InteractableInView != PreviousInteractableInView)
	{
		if (InteractableInView)
		{
			InteractableInView->SetHoverWidgetVisibility(true);
		}
		if (PreviousInteractableInView)
		{
			PreviousInteractableInView->SetHoverWidgetVisibility(false);
		}
	}
	if (ToyInView != PreviousToyInView)
	{
		if (ToyInView)
		{
			ToyInView->SetHoverWidgetVisibility(true);
			// ToyInView->Highlight(true);
		}
		if (PreviousToyInView)
		{
			PreviousToyInView->SetHoverWidgetVisibility(false);
			// PreviousToyInView->Highlight(false);
		}
	}
	if ((!bHasInteractableInView && !bHasToyInView) && (bHadInteractableInView || bHadToyInView))
	{
		OnEndAnyInView.Broadcast();
	}
}

AToy* ALukaCharacter::GetToyInView() const
{
	return ToyInView;
}

void ALukaCharacter::Interact()
{
	if (InteractableInView)
	{
		
		if (AObji* Obji = Cast<AObji>(InteractableInView))
		{
			if (!HasObji())
			{
				PickupObji(Obji);
			}
		}
		else
		{
			InteractableInView->Interact(this);
		}
	}
}

void ALukaCharacter::PickupObji(AObji* Obji)
{
	check(HandComponent);
	check(Obji);
	Obji->Interact(this);
	HandComponent->PickupObji(Obji);
	OnEndAnyInView.Broadcast();
	OnEndSpecificInView.Broadcast(Obji);
}

AInteractable* ALukaCharacter::GetInteractableInView() const
{
	return InteractableInView;
}

AObji* ALukaCharacter::DropObji()
{
	check(HandComponent);

	InteractableInView = nullptr;
	ToyInView = nullptr;

	if (AObji* Obji = HandComponent->ReleaseObji())
	{
		return Obji;
	}
	return nullptr;
}

void ALukaCharacter::SnapToGround(const float DistanceCheck)
{
	FHitResult HitResult;
	const FVector StartLocation = GetActorLocation();
	const FVector EndLocation = StartLocation + FVector(0.f, 0.f, -DistanceCheck);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, FloorCollisionChannel,
	                                                 QueryParams);

	if (bHit == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		                                 TEXT(
			                                 "No Ground Found! Don't forget to add trace channel 'Snap Surface' to the ground component"));
		return;
	}

	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	SetActorLocation(HitResult.Location + FVector(0.f, 0.f, CapsuleHalfHeight + 2.f));
}

void ALukaCharacter::LookAtToy(const AToy* Toy)
{
	check(Toy != nullptr);
	check(CameraComponent != nullptr);

	FRotator NewRot = Toy->GetCameraComponent()->GetComponentRotation();
	NewRot = FRotator(0, NewRot.Yaw, 0);
	SetActorRotation(NewRot);
	CameraComponent->SetRelativeRotation(FRotator::ZeroRotator);
}

void ALukaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!ensure(EnhancedInput != nullptr))
	{
		return;
	}

	if (ensure(InputActionMove != nullptr))
	{
		EnhancedInput->BindAction(InputActionMove, ETriggerEvent::Triggered, this, &ALukaCharacter::Move);
		EnhancedInput->BindAction(InputActionMove, ETriggerEvent::Started, this, &ALukaCharacter::HandleStartMoving);
		EnhancedInput->BindAction(InputActionMove, ETriggerEvent::Completed, this, &ALukaCharacter::HandleStopMoving);
	}
	if (ensure(InputActionLook != nullptr))
	{
		EnhancedInput->BindAction(InputActionLook, ETriggerEvent::Triggered, this, &ALukaCharacter::Look);
	}
}
