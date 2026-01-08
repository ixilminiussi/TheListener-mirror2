#include "GPE/Radio/Decoder.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/TextRenderComponent.h"
#include "GPE/Radio/KnobComponent.h"
#include "GPE/Radio/PrimaryStation.h"
#include "GPE/Radio/Radio.h"
#include "GPE/Radio/StationDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Miscellaneous/TLUtils.h"
#include "Player/LukaController.h"
#include "UI/Prompt/CommandHUDComponent.h"

ADecoder::ADecoder()
{
	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	SetRootComponent(BodyMesh);
	check(BodyMesh);

	LeftKnobComponent = CreateDefaultSubobject<UKnobComponent>(TEXT("LeftKnob"));
	LeftKnobComponent->SetupAttachment(BodyMesh);
	check(LeftKnobComponent);
	RightKnobComponent = CreateDefaultSubobject<UKnobComponent>(TEXT("RightKnob"));
	RightKnobComponent->SetupAttachment(BodyMesh);
	check(RightKnobComponent);

	CommandHUDComponent = CreateDefaultSubobject<UCommandHUDComponent>(TEXT("CommandHUDComponent"));
	check(CommandHUDComponent);
}

void ADecoder::BeginPlay()
{
	Super::BeginPlay();

	// Setup Knob
	if (!ensure(LeftKnobComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("Missing Left Knob Component"));
		return;
	}
	LeftKnobComponent->SetRange(ValuesRange);

	if (!ensure(RightKnobComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("Missing Right Knob Component"));
		return;
	}
	RightKnobComponent->SetRange(ValuesRange);

	check(CommandHUDComponent);
	check(DecoderInputMappingContext);
	CommandHUDComponent->Generate(DecoderInputMappingContext);

	if (ensure(RunningMaterial) && ensure(BodyMesh))
	{
		RunningMaterialInstance = UMaterialInstanceDynamic::Create(RunningMaterial, this);
	}
	ResetMaterial();
}

void ADecoder::SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (ensure(InputActionSpinLeftKnob))
	{
		EnhancedInputComponent->BindAction(InputActionSpinLeftKnob, ETriggerEvent::Triggered, this,
		                                   &ADecoder::SpinLeftKnob);
	}
	if (ensure(InputActionSpinRightKnob))
	{
		EnhancedInputComponent->BindAction(InputActionSpinRightKnob, ETriggerEvent::Triggered, this,
		                                   &ADecoder::SpinRightKnob);
	}
}

void ADecoder::OnEndToyPossessEvent()
{
	SetActiveWidgetVisibility(false);
}

void ADecoder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ADecoder::StartDecoder(ALukaController* Controller)
{
	float LTarget = FMath::RandRange(ValuesRange.X, ValuesRange.Y);
	float RTarget = FMath::RandRange(ValuesRange.X, ValuesRange.Y);
	float HardRNG = FMath::RandRange(0, 100);
	return StartDecoder(Controller,LTarget,RTarget,HardRNG<50.0f);
}


bool ADecoder::StartDecoder(const ALukaController* Controller, const float InTargetLeft, const float InTargetRight, const bool bComplex)
{
	if (!bEnabled)
	{
		return false;
	}
	TargetLeft = InTargetLeft;
	TargetRight = InTargetRight;

	if (!DecoderInputMappingContext)
	{
		return false;
	}

	if (Controller)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = Controller->GetLocalPlayer()->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>();
		InputSubsystem->AddMappingContext(DecoderInputMappingContext, 10);
	}
	
	check(LeftKnobComponent);
	check(RightKnobComponent);
	LeftKnobComponent->SetValue(0.f);
	RightKnobComponent->SetValue(0.f);

	if (ensure(BodyMesh) && ensure(RunningMaterialInstance))
	{
		RunningMaterialInstance->SetVectorParameterValue("Freq_T", FLinearColor{
			                                                 InTargetLeft / Ratio,
			                                                 InTargetRight / Ratio, 0.f, 0.f
		                                                 });
		RunningMaterialInstance->SetVectorParameterValue("Freq_H", FLinearColor{
			                                                 LeftKnobComponent->GetValue() / Ratio,
			                                                 RightKnobComponent->GetValue() / Ratio, 0.f, 0.f
		                                                 });
		RunningMaterialInstance->SetScalarParameterValue("IsDifficultyHard", bComplex ? 1 : 0);
	}

	OnLeftToggle(false);
	OnRightToggle(false);

	SetActiveWidgetVisibility(true);

	return true;
}

void ADecoder::StopDecoder()
{
	RemoveMappingContext();

	ResetMaterial();
}

void ADecoder::Toggle(bool bToggle)
{
	bEnabled = bToggle;

	check(BodyMesh);
	BodyMesh->SetVisibility(bToggle);
	check(LeftKnobComponent);
	LeftKnobComponent->SetVisibility(bToggle);
	check(RightKnobComponent);
	RightKnobComponent->SetVisibility(bToggle);

	OnToggle.Broadcast(bToggle);
}

void ADecoder::RemoveMappingContext() const
{
	if (!DecoderInputMappingContext)
	{
		return;
	}

	if (const APlayerController* PlayerController = Cast<APlayerController>(
		UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>();
		InputSubsystem->RemoveMappingContext(DecoderInputMappingContext);
	}
	SetActiveWidgetVisibility(false);
}

void ADecoder::SpinLeftKnob(const struct FInputActionValue& Value)
{
	const FVector2d RecordedValue = Value.Get<FVector2D>();
	const float Length = RecordedValue.Length();
	const float Angle = UTLUtils::GetAngleFrom2DVector(RecordedValue);

	if (bIsNotFirstLeftRollAngle)
	{
		if (Length < MinimumRollInputRegister)
		{
			bIsNotFirstLeftRollAngle = false;
		}
		else
		{
			const float RelativeValue = FMath::RadiansToDegrees(UTLUtils::GetAngleDelta(Angle, LastLeftAngle)) / 360.f;
			LeftKnobComponent->ClockwiseRotate(RelativeValue, 1.0f);
			RunningMaterialInstance->SetVectorParameterValue("Freq_H", FLinearColor{
				                                                 LeftKnobComponent->GetValue() / Ratio,
				                                                 RightKnobComponent->GetValue() / Ratio, 0.f, 0.f
			                                                 });
			LastLeftAngle = Angle;
			
			IsLeftDone = CheckTarget(TargetLeft, LeftKnobComponent->GetValue());
			if (IsLeftDone)
			{
				OnLeftToggle(true);
				CheckAll(true);
			}
			else
			{
				OnLeftToggle(false);

				GetWorldTimerManager().ClearTimer(DelayHandle);
				DelayHandle.Invalidate();
				PendingLaunching = false;
			}
		}
	}
	else
	{
		if (Length >= MinimumRollInputRegister)
		{
			bIsNotFirstLeftRollAngle = true;
			LastLeftAngle = Angle;
		}
	}
}

void ADecoder::SpinRightKnob(const struct FInputActionValue& Value)
{
	const FVector2d RecordedValue = Value.Get<FVector2D>();
	const float Length = RecordedValue.Length();
	const float Angle = UTLUtils::GetAngleFrom2DVector(RecordedValue);

	if (bIsNotFirstRightRollAngle)
	{
		if (Length < MinimumRollInputRegister)
		{
			bIsNotFirstRightRollAngle = false;
		}
		else
		{
			const float RelativeValue = FMath::RadiansToDegrees(UTLUtils::GetAngleDelta(Angle, LastRightAngle)) / 360.f;
			RightKnobComponent->ClockwiseRotate(RelativeValue, 1.0f);
			RunningMaterialInstance->SetVectorParameterValue("Freq_H", FLinearColor{
				                                                 LeftKnobComponent->GetValue() / Ratio,
				                                                 RightKnobComponent->GetValue() / Ratio, 0.f, 0.f
			                                                 });
			LastRightAngle = Angle;
			
			IsRightDone = CheckTarget(TargetRight, RightKnobComponent->GetValue());
			if (IsRightDone)
			{

				OnRightToggle(true);
				CheckAll(false);
			}
			else
			{
				OnRightToggle(false);

				GetWorldTimerManager().ClearTimer(DelayHandle);
				DelayHandle.Invalidate();
				PendingLaunching = false;
			}
		}
	}
	else
	{
		if (Length >= MinimumRollInputRegister)
		{
			bIsNotFirstRightRollAngle = true;
			LastRightAngle = Angle;
		}
	}
}

FVector2d ADecoder::GetFrequencyRange() const
{
	return ValuesRange;
}

bool ADecoder::CheckTarget(float TargetFrequency, float CurrentValue) const
{
	float Difference = FMath::Abs(TargetFrequency) - FMath::Abs(CurrentValue);
	if (FMath::Abs(Difference) < TargetsThreshold)
	{
		return true;
	}
	return false;
}

void ADecoder::CheckAll(bool isLeftDone)
{
	float OtherTarget = IsLeftDone ? TargetRight : TargetLeft;
	float OtherCurrent = isLeftDone ? RightKnobComponent->GetValue() : LeftKnobComponent->GetValue();
	if (CheckTarget(OtherCurrent, OtherTarget) && !DelayHandle.IsValid())
	{
		FTimerDelegate DecodeCompleteLaunchTimerDel;
		DecodeCompleteLaunchTimerDel.BindUFunction(this, "CompleteObjective");
		GetWorld()->GetTimerManager().SetTimer(DelayHandle, DecodeCompleteLaunchTimerDel, ValidationDelay,false);
	}
}

void ADecoder::CompleteObjective()
{
	StopDecoder();
	OnFinish.Broadcast();
	
}

void ADecoder::ResetMaterial() const
{
	if (ensure(BodyMesh) && ensure(RunningMaterialInstance))
	{
		BodyMesh->SetMaterial(3, RunningMaterialInstance);

		RunningMaterialInstance->SetVectorParameterValue("Freq_T", FLinearColor{
			                                                 0.f, 0.f, 0.f, 0.f
		                                                 });
		RunningMaterialInstance->SetVectorParameterValue("Freq_H", FLinearColor{
			                                                 0.f, 0.f, 0.f, 0.f
		                                                 });
	}
}

void ADecoder::SetActiveWidgetVisibility(const bool bIsVisible) const
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
