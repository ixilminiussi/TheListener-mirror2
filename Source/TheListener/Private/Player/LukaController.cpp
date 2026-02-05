#include "Player/LukaController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Player/LukaCharacter.h"
#include "GPE/Toy.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Components/PickupComponent.h"
#include "Structs/PlayerData.h"
#include "UI/LukaHUD.h"
#include "UI/Prompt/HoverPromptComponent.h"
#include "UserSettings/EnhancedInputUserSettings.h"

ALukaController::ALukaController()
{
}

void ALukaController::BeginPlay()
{
	Super::BeginPlay();
	LukaHUD = Cast<ALukaHUD>(GetHUD());
}

void ALukaController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Bind to Subsystem;
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!ensure(EnhancedInputSubsystem != nullptr))
	{
		return;
	}
	
	UEnhancedInputUserSettings* UserSettings = EnhancedInputSubsystem->GetUserSettings();
	if (UserSettings)
	{
		UserSettings->LoadOrCreateSettings(GetLocalPlayer());
		UserSettings->ApplySettings();
	}

	EnhancedInputSubsystem->ClearAllMappings();
	EnhancedInputSubsystem->AddMappingContext(GlobalInputMappingContext, 0);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!ensure(EnhancedInputSubsystem != nullptr))
	{
		return;
	}

	EnhancedInputComponent->ClearActionBindings();

	if (ensure(InputActionInteract != nullptr))
	{
		EnhancedInputComponent->BindAction(InputActionInteract, ETriggerEvent::Triggered, this,
		                                   &ALukaController::Interact);
	}
	if (ensure(InputActionReturn != nullptr))
	{
		EnhancedInputComponent->BindAction(InputActionReturn, ETriggerEvent::Triggered, this, &ALukaController::Return);
	}
	if (ensure(InputActionReturn != nullptr))
	{
		EnhancedInputComponent->BindAction(InputActionDrop, ETriggerEvent::Triggered, this, &ALukaController::Drop);
	}
	if (ensure(InputActionPause != nullptr))
	{
		EnhancedInputComponent->BindAction(InputActionPause, ETriggerEvent::Triggered, this,
		                                   &ALukaController::PauseGame);
	}
}

void ALukaController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (Luka != nullptr)
	{
		return;
	}

	Luka = Cast<ALukaCharacter>(InPawn);
	check(Luka);
}

UInputMappingContext* ALukaController::GetGlobalMappingContext() const
{
	return GlobalInputMappingContext;
}

ALukaCharacter* ALukaController::GetLukaCharacter() const
{
	check(Luka);
	return Luka;
}

void ALukaController::KickFromCurrentToy()
{
	if (PossessedToy) ReleaseToy(PossessedToy);
	else UE_LOG(LogTemp, Warning, TEXT("Not in a toy"));
}

void ALukaController::Interact(const FInputActionValue& Value)
{
	check(Luka != nullptr);

	if (AToy* Toy = Luka->GetToyInView())
	{
		PossessToy(Toy);
	} else
	{
		Luka->Interact();
	}
}

void ALukaController::Return(const FInputActionValue& Value)
{
	if (bTransitionMutex)
	{
		return;
	}

	if (AToy* Toy = Cast<AToy>(GetPawn()))
	{
		ReleaseToy(Toy);
	} else
	{
		check(Luka != nullptr);

		Luka->Return();
	}
}

void ALukaController::Drop(const FInputActionValue& Value)
{
	if (Luka)
	{
		Luka->Drop();
	}
}

void ALukaController::MovePlayerBehindToy(const AToy* Toy) const
{
	check(Toy != nullptr);
	check(Luka != nullptr);

	const UCameraComponent* Camera = Toy->GetCameraComponent();
	if (!ensure(Camera != nullptr))
	{
		return;
	}

	const FVector CameraLocation = Camera->GetComponentLocation();
	const FVector CameraForward = Camera->GetForwardVector();

	const FVector EndLocation = CameraLocation + CameraForward * -Toy->GetCameraDistanceBack() + FVector(
		0.f, 0.f, 5000.f);

	Luka->SetActorLocation(EndLocation);
	Luka->SnapToGround(10000.f);
	Luka->LookAtToy(Toy);
}

void ALukaController::PauseGame(const struct FInputActionValue& Value)
{
	if (ensure(LukaHUD != nullptr))
	{
		if (!UGameplayStatics::IsGamePaused(GetWorld()))
		{
			LukaHUD->PauseGame();
		}
	}
}

void ALukaController::PossessToy(AToy* Toy)
{
	if (!Toy->IsEnabled())
	{
		return;
	}

	if (UHoverPromptComponent *HoverComponent = Toy->GetComponentByClass<UHoverPromptComponent>(); HoverComponent)
	{
		HoverComponent->TogglePrompts(false);
	}

	if (Luka != nullptr)
	{
		if (Luka->IsPawnControlled())
		{
			Luka->DisableInput(this);
		}
	}
	OnPossessToyTransition.Broadcast();
	SetViewTargetWithBlend(Toy, CameraBlendTime, VTBlend_EaseInOut, TransitionExponential);
	PossessedToy = Toy;
	PossessedToy->OnPossessToyTransition();
	bTransitionMutex = true;

	UPickupComponent *LukaPickupComponent = Luka->GetComponentByClass<UPickupComponent>();
	UPickupComponent *ToyPickupComponent = Toy->GetComponentByClass<UPickupComponent>();

	if (LukaPickupComponent && ToyPickupComponent)
	{
		LukaPickupComponent->Pass(ToyPickupComponent);
	}
	
	GetWorldTimerManager().SetTimer(CameraDelayHandle, this, &ALukaController::EnactPossession, CameraBlendTime,
	                                       false);

	if (LukaHUD)
	{
		LukaHUD->ToggleCursorVisibility(false);
	}
}

void ALukaController::EnactPossession()
{
	if (PossessedToy != nullptr)
	{
		Possess(PossessedToy);
		OnPossessToy.Broadcast();
	}
	else
	{
		Possess(GetLukaCharacter());
		OnUnpossessToy.Broadcast();
	}
	bTransitionMutex = false;
}

void ALukaController::ReleaseToy(AToy* Toy)
{
	if (Toy->IsLocked())
	{
		return;
	}
	OnUnpossessToyTransition.Broadcast();
	Toy->OnUnpossessToyTransition();
	Toy->DisableInput(this);
	MovePlayerBehindToy(Toy);
	SetViewTargetWithBlend(Luka, CameraBlendTime, VTBlend_EaseInOut, 3.0f);
	PossessedToy = nullptr;
	bTransitionMutex = true;

	UPickupComponent *LukaPickupComponent = Luka->GetComponentByClass<UPickupComponent>();
	UPickupComponent *ToyPickupComponent = Toy->GetComponentByClass<UPickupComponent>();

	if (LukaPickupComponent && ToyPickupComponent)
	{
		ToyPickupComponent->Pass(LukaPickupComponent);
	}
	
	GetWorldTimerManager().SetTimer(CameraDelayHandle, this, &ALukaController::EnactPossession, CameraBlendTime,
	                                       false);
	
	if (LukaHUD)
	{
		LukaHUD->ToggleCursorVisibility(true);
	}
}

void ALukaController::SetGlobalInputsStatus(bool IsRunning) const
{
	if (!GlobalInputMappingContext)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (IsRunning)
	{
		EnhancedInputSubsystem->AddMappingContext(GlobalInputMappingContext, 0);
	}
	else
	{
		EnhancedInputSubsystem->RemoveMappingContext(GlobalInputMappingContext);
	}
}

void ALukaController::MapPlayerData(const UPlayerData* InPlayerData)
{
	check(InPlayerData);

	PlayerCameraManager->ViewPitchMax = InPlayerData->ClampUp;
	PlayerCameraManager->ViewPitchMin = InPlayerData->ClampDown;

	CameraBlendTime = InPlayerData->CameraBlendTime;
	TransitionExponential = InPlayerData->TransitionExponential;
}
