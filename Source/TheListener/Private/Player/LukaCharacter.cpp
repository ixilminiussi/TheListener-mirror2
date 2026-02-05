#include "Player/LukaCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GPE/Toy.h"
#include "Player/LukaController.h"
#include "Player/Components/InspectingComponent.h"
#include "Player/Components/InteractComponent.h"
#include "Player/Components/PickupComponent.h"
#include "Structs/PlayerData.h"
#include "System/Core/BaseGameInstance.h"
#include "UI/LukaHUD.h"
#include "UI/Menus/SettingsSave.h"
#include "UI/Prompt/HoverPromptComponent.h"

ALukaCharacter::ALukaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	PickupComponent = CreateDefaultSubobject<UPickupComponent>("Pickup");
	InteractComponent = CreateDefaultSubobject<UInteractComponent>("Interact");
	DropComponent = CreateDefaultSubobject<USceneComponent>("Drop");
	check(RootComponent);
	check(SpringArmComponent);
	check(CameraComponent);
	check(PickupComponent);
	check(InteractComponent);
	check(DropComponent);

	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = true;
	DropComponent->SetupAttachment(CameraComponent);
	PickupComponent->SetupAttachment(CameraComponent);
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

USceneComponent* ALukaCharacter::GetDrop() const
{
	return DropComponent;
}

void ALukaCharacter::InteractiveRayCheck()
{
	AActor* NewActorInView = nullptr;

	// Check if hitting Hitbox == BEGIN =====================
	TArray<FHitResult> HitResults;
	const FVector StartLocation = CameraComponent->GetComponentLocation();
	const FVector EndLocation = StartLocation + CameraComponent->GetForwardVector() * InteractionMaxRange;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(InteractiveCollisionChannel);

	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(LineTrace), true, this);

	bool bHit = GetWorld()->LineTraceMultiByObjectType(HitResults, StartLocation, EndLocation, ObjectQueryParams,
	                                                   TraceParams);

	if (!bHit)
	{
		ALukaHUD::InteractiveInView.Execute(false);
	} else
	{
		FHitResult WallHitResult;
		const bool bWallHit = GetWorld()->LineTraceSingleByChannel(WallHitResult, StartLocation, EndLocation,
																   ECC_WorldStatic,
																   TraceParams);

		if (bWallHit)
		{
			const int Index = HitResults.IndexOfByPredicate([&WallHitResult](const FHitResult& Hit) { return Hit.Distance > WallHitResult.Distance; });
			if (Index >= 0)
			{
				HitResults.RemoveAt(Index, HitResults.Num() - Index);
			}
		}
	
		// Check if hitting Walls == END ========================
		NewActorInView = PickupComponent->CheckAndFilterHitResults(&HitResults);
		if (!NewActorInView && HitResults.Num() > 0)
		{
			NewActorInView = InteractComponent->CheckHitResults(&HitResults, PickupComponent->GetHeld());
			if (!NewActorInView && HitResults.Num() > 0)
			{
				TArray<FHitResult> Filtered = HitResults.FilterByPredicate([](const FHitResult& Hit)
				{
					AToy *Toy = Cast<AToy>(Hit.GetActor());
					if (Toy != nullptr)
					{
						return Toy->IsEnabled();
					} return false;
				});
				if (Filtered.Num() > 0)
				{
					NewActorInView = Filtered[0].GetActor();
				}
			}
		}
	}
	
	ALukaHUD::InteractiveInView.Execute(NewActorInView != nullptr);

	if (NewActorInView != ActorInView)
	{
		TryShowHoverPrompt(NewActorInView);
		TryHideHoverPrompt(ActorInView);
	}

	ActorInView = NewActorInView;
}

AToy* ALukaCharacter::GetToyInView() const
{
	if (ActorInView == nullptr)
	{
		return nullptr;
	}
	return Cast<AToy>(ActorInView);
}

void ALukaCharacter::TryShowHoverPrompt(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}
	UHoverPromptComponent *HoverPromptComponent = Actor->FindComponentByClass<UHoverPromptComponent>();

	if (HoverPromptComponent)
	{
		HoverPromptComponent->TogglePrompts(true);
	}
}

void ALukaCharacter::TryHideHoverPrompt(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}
	UHoverPromptComponent *HoverPromptComponent = Actor->FindComponentByClass<UHoverPromptComponent>();

	if (HoverPromptComponent)
	{
		HoverPromptComponent->TogglePrompts(false);
	}
}

void ALukaCharacter::Interact() const
{
	check(InteractComponent);
	check(PickupComponent);
	if (InteractComponent->Interact(ActorInView, PickupComponent->GetHeld()))
	{
		PickupComponent->Consume();
	} else
	{
		PickupComponent->TryPickup(ActorInView);
	}
}

void ALukaCharacter::Return() const
{
}

void ALukaCharacter::Drop() const
{
	check(PickupComponent);
	PickupComponent->TryDrop();
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

	if (UInspectingComponent *InspectingComponent = GetComponentByClass<UInspectingComponent>())
	{
		InspectingComponent->SetupInput(EnhancedInput);
	}
}
