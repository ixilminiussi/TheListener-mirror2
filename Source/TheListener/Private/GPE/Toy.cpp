#include "GPE/Toy.h"

#include "AkComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/LocalPlayer.h"
#include "UI/Prompt/CommandHUDComponent.h"
#include "EventSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LukaCharacter.h"
#include "Player/LukaController.h"
#include "System/Events/EventCondition.h"

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

	CommandHUDComponent = CreateDefaultSubobject<UCommandHUDComponent>("CommandHUDComponent");

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

	if (ensure(InputActionLook))
	{
		EnhancedInput->BindAction(InputActionLook, ETriggerEvent::Triggered, this, &AToy::Look);
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
			SetActiveWidgetVisibility(false);
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
			SetActiveWidgetVisibility(true);
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

	check(CommandHUDComponent);

	if (CameraComponent)
	{
		StartingRotation = CameraComponent->GetRelativeRotation();
	}
	
	CommandHUDComponent->Generate(InputMappingContext);
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

	if (UEventSubsystem* EventSubsystem = GetWorld()->GetSubsystem<UEventSubsystem>(); ensure(EventSubsystem))
	{
		const FConditionKey Key = UToyPossessedCondition::GenerateKey(GetClass());
		EventSubsystem->SetConditionValue(Key, true, true);
	}

	SetActiveWidgetVisibility(true);
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

		const auto EventSubsystem = GetWorld()->GetSubsystem<UEventSubsystem>();
		check(EventSubsystem);
		EventSubsystem->SetConditionValue(Key, false, true);
	}

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
	}

	if (!bLooking)
	{
		TargetRotation = StartingRotation; 
	}
	bLooking = false;
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
	SetHoverWidgetVisibility(false);
	if (OnPossessTransitionAkEvent)
	{
		check(AkComponent);
		FOnAkPostEventCallback NullCallback;
		AkComponent->PostAkEvent(OnPossessTransitionAkEvent, 0, NullCallback);
	}
}

void AToy::OnUnpossessToyTransition()
{
	SetActiveWidgetVisibility(false);
	if (OnUnpossessTransitionAkEvent)
	{
		check(AkComponent);
		FOnAkPostEventCallback NullCallback;
		AkComponent->PostAkEvent(OnUnpossessTransitionAkEvent, 0, NullCallback);
	}
}

void AToy::SetHoverWidgetVisibility(const bool bIsVisible) const
{
	if (!bEnabled)
	{
		return;
	}

	check(CommandHUDComponent);
	if (bIsVisible)
	{
		CommandHUDComponent->AddHoverToHUD();
	}
	else
	{
		CommandHUDComponent->RemoveHoverFromHUD();
	}
}

void AToy::SetActiveWidgetVisibility(const bool bIsVisible) const
{
	check(CommandHUDComponent);
	if (bIsVisible)
	{
		CommandHUDComponent->AddActiveToHUD();
	}
	else
	{
		CommandHUDComponent->RemoveActiveFromHUD();
	}
}
