#include "GPE/Toy.h"

#include "AkComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/LocalPlayer.h"
#include "EventSubsystem.h"
#include "Miscellaneous/TLUtils.h"
#include "Player/LukaController.h"
#include "Player/Components/InspectingComponent.h"
#include "System/Events/EventCondition.h"
#include "TheListener/TheListener.h"

AToy::AToy()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>("Collision");
	check(CollisionComponent);

	SetRootComponent(CollisionComponent);
	CollisionComponent->SetCollisionProfileName(FName("Hitbox"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	check(CameraComponent);

	CameraComponent->SetupAttachment(CollisionComponent);

	AkComponent = CreateDefaultSubobject<UAkComponent>("AkComponent");
	AkComponent->SetupAttachment(RootComponent);
	AkComponent->SetRelativeLocation(FVector::ZeroVector);
	check(AkComponent);
}

void AToy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!ensure(EnhancedInput))
	{
		return;
	}

	if (InputActionCameraZoom)
	{
		EnhancedInput->BindAction(InputActionCameraZoom, ETriggerEvent::Triggered, this, &AToy::Zoom);
	}

	if (InputActionCameraLook)
	{
		EnhancedInput->BindAction(InputActionCameraLook, ETriggerEvent::Triggered, this, &AToy::Look);
	}

	if (UInspectingComponent *InspectingComponent = GetComponentByClass<UInspectingComponent>())
	{
		InspectingComponent->SetupInput(EnhancedInput);
	}
}

const UCameraComponent* AToy::GetCameraComponent() const
{
	return CameraComponent;
}

void AToy::Lock(const bool bEnable)
{
	if (!InputMappingContext)
	{
		return;
	}

	bLocked = bEnable;

	if (bLocked) // remove InputMappingContext
	{
		if (const APlayerController* PlayerController = Cast<APlayerController>(
			GetController()))
		{
			UEnhancedInputLocalPlayerSubsystem* InputSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<
				UEnhancedInputLocalPlayerSubsystem>();

			InputSubsystem->RemoveMappingContext(InputMappingContext);
			ShowActivePrompts(false);
		}
	}
	else // bring back InputMappingContext
	{
		if (const APlayerController* PlayerController = Cast<APlayerController>(
			GetController()))
		{
			UEnhancedInputLocalPlayerSubsystem* InputSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<
				UEnhancedInputLocalPlayerSubsystem>();

			InputSubsystem->AddMappingContext(InputMappingContext, 1);
			ShowActivePrompts(true);
		}
	}
}

bool AToy::IsLocked() const
{
	return bLocked;
}

bool AToy::IsEnabled() const
{
	return bEnabled;
}

void AToy::Enable(const bool bToggle)
{
	bEnabled = bToggle;
}

void AToy::Highlight(const bool bToggle)
{
	if (!IsEnabled())
	{
		return;
	}

	static bool LastCall = false;
	if (bToggle == LastCall)
	{
		return;
	}
	LastCall = bToggle;

	TArray<UStaticMeshComponent*> StaticMeshComponents;
	GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	if (!ensure(HighlightMaterial))
	{
		return;
	}
	for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
	{
		if (bToggle)
		{
			StaticMeshComponent->SetOverlayMaterial(HighlightMaterial);
		}
		else
		{
			StaticMeshComponent->SetOverlayMaterial(nullptr);
		}
	}
}

void AToy::KickPlayer()
{
	if (ALukaController* LukaController = Cast<ALukaController>(GetController()); LukaController)
	{
		LukaController->KickFromCurrentToy();
	}
}

void AToy::BeginPlay()
{
	Super::BeginPlay();

	if (CameraComponent)
	{
		StartingRotation = CameraComponent->GetRelativeRotation();
		StartingZoom = CameraComponent->FieldOfView;
		TargetZoom = StartingZoom / ZoomAmount;
	}
}

void AToy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APlayerController* PlayerController = Cast<APlayerController>(NewController);
	if (!PlayerController)
	{
		return;
	}

	EnableInput(PlayerController);
	bIsPossessed = true;

	if (!InputMappingContext)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	InputSubsystem->AddMappingContext(InputMappingContext, 2);
	InputSubsystem->AddMappingContext(FunMappingContext, 1);
	ShowActivePrompts(true);

	if (UEventSubsystem* EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UEventSubsystem>(); ensure(EventSubsystem))
	{
		const FConditionKey Key = UToyPossessedCondition::GenerateKey(GetClass());
		EventSubsystem->SetConditionValue(Key, true, true);
	}

	OnPossessAfterTransition();
}

void AToy::UnPossessed()
{
	bIsPossessed = false;

	if (!InputMappingContext)
	{
		return;
	}

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>();
		InputSubsystem->RemoveMappingContext(InputMappingContext);

		const FConditionKey Key = UToyPossessedCondition::GenerateKey(GetClass());

		const auto EventSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UEventSubsystem>();
		check(EventSubsystem);
		EventSubsystem->SetConditionValue(Key, false, true);
	}
	OnUnPossessAfterTransition();
	Super::UnPossessed();
}

void AToy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CameraComponent)
	{
		FRotator TargetRotator = TargetRotation;
		FRotator CurrentRotator = CameraComponent->GetRelativeRotation();
		if (!CurrentRotator.Equals(TargetRotator, 0.001f))
		{
			FRotator DiffRotation = FMath::RInterpTo(CurrentRotator, TargetRotator, DeltaTime, LookLerpStrength);

			CameraComponent->SetRelativeRotation(DiffRotation);
		}

		float FOV = CameraComponent->FieldOfView;
		if (!FMath::IsNearlyEqual(FOV, TargetZoom))
		{
			float NewFOV;
			
			if (bZooming)
			{
				NewFOV = FMath::FInterpTo(FOV, TargetZoom, DeltaTime, ZoomLerpStrength);
			} else
			{
				NewFOV = FMath::FInterpTo(FOV, StartingZoom, DeltaTime, ZoomLerpStrength);
			}

			CameraComponent->SetFieldOfView(NewFOV);
		}
	}

	if (!bLooking)
	{
		TargetRotation = StartingRotation; 
	}
	bLooking = false;
}

void AToy::Zoom(const struct FInputActionValue& Value)
{
	LOG("testing %f", Value.Get<float>());
	bZooming = Value.IsNonZero();
}

void AToy::Look(const struct FInputActionValue& Value)
{
	TargetRotation = StartingRotation;
	TargetRotation.Yaw += Value.Get<FVector2D>().X * MaxRotation.X;
	TargetRotation.Pitch += Value.Get<FVector2D>().Y * MaxRotation.Y;

	bLooking = true;
}

float AToy::GetCameraDistanceBack() const
{
	return CameraDistanceBack;
}

void AToy::OnPossessToyTransition()
{
	if (OnPossessTransitionAkEvent)
	{
		check(AkComponent);
		FOnAkPostEventCallback NullCallback;
		AkComponent->PostAkEvent(OnPossessTransitionAkEvent, 0, NullCallback);
	}
	OnPossessBeforeTransition();
}

void AToy::OnUnpossessToyTransition()
{
	ShowActivePrompts(false);
	OnUnPossessBeforeTransition();
}

void AToy::ShowActivePrompts(const bool bIsVisible) const
{
	if (bIsVisible)
	{
		FTimerDelegate TimerDelegate;
		TWeakObjectPtr<UWorld> World = GetWorld();
		TimerDelegate.BindLambda([World, this]()
		{
			if (World.IsValid())
			{
				UTLUtils::TogglePrompts(World.Get(), OnActivePrompts, true);
			}
		});
		GetWorldTimerManager().SetTimerForNextTick(TimerDelegate);
	} else
	{
		FTimerDelegate TimerDelegate;
		TWeakObjectPtr<UWorld> World = GetWorld();
		TimerDelegate.BindLambda([World, this]()
		{
			if (World.IsValid())
			{
				UTLUtils::TogglePrompts(World.Get(), OnActivePrompts, false);
			}
		});
		GetWorldTimerManager().SetTimerForNextTick(TimerDelegate);
	}
}

void AToy::OnUnPossessAfterTransition_Implementation()
{
}

void AToy::OnUnPossessBeforeTransition_Implementation()
{
}

void AToy::OnPossessAfterTransition_Implementation()
{
}

void AToy::OnPossessBeforeTransition_Implementation()
{
}
