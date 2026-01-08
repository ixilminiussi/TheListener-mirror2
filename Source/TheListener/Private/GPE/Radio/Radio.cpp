#include "GPE/Radio/Radio.h"
#include "EnhancedInputComponent.h"
#include "AkGameplayStatics.h"
#include "AkGameplayTypes.h"
#include "AkComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/WidgetComponent.h"
#include "Player/LukaController.h"
#include "GPE/Radio/KnobComponent.h"
#include "GPE/Radio/Decoder.h"
#include "GPE/Radio/ScreenComponent.h"
#include "GPE/Radio/StationDataAsset.h"
#include "GPE/Radio/Station.h"
#include "Kismet/GameplayStatics.h"
#include "System/Frequency/FrequencySubsystem.h"
#include "UI/RadioText.h"

ARadio::ARadio()
{
	PrimaryActorTick.bCanEverTick = true;

	check(RootComponent);
	SoundPosition = CreateDefaultSubobject<USceneComponent>("SoundPosition");
	SoundPosition->SetupAttachment(RootComponent);
	check(SoundPosition);

	RadioAkComponent = CreateDefaultSubobject<UAkComponent>("RadioAkComponent");
	RadioAkComponent->SetupAttachment(SoundPosition);
	RadioAkComponent->SetRelativeLocation(FVector::ZeroVector);
	check(RadioAkComponent);

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	BaseMeshComponent->SetupAttachment(RootComponent);
	check(BaseMeshComponent);

	KnobComponent = CreateDefaultSubobject<UKnobComponent>("Knob");
	KnobComponent->SetupAttachment(BaseMeshComponent);
	check(KnobComponent);

	DecoderComponent = CreateDefaultSubobject<UChildActorComponent>("DecoderComponent");
	DecoderComponent->SetupAttachment(RootComponent);
	check(DecoderComponent);

	ScreenComponent = CreateDefaultSubobject<UChildActorComponent>("ScreenComponent");
	ScreenComponent->SetupAttachment(RootComponent);
	check(ScreenComponent);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>("FrequencyDisplay");
	WidgetComponent->SetupAttachment(RootComponent);
	check(WidgetComponent);

	if (DecoderComponentClass)
	{
		DecoderComponent->SetChildActorClass(DecoderComponentClass);
	}
	if (ScreenComponentClass)
	{
		ScreenComponent->SetChildActorClass(ScreenComponentClass);
	}
}

void ARadio::BeginPlay()
{
	Super::BeginPlay();

	UFrequencySubsystem *FrequencySubsystem = GetWorld()->GetSubsystem<UFrequencySubsystem>();
	if (ensure(FrequencySubsystem))
	{
		FrequencySubsystem->RegisterReceiver(this);
	}

	// Setup Knob
	if (!ensure(KnobComponent))
	{
		UE_LOG(LogTemp, Error,
		       TEXT("Missing Knob Component"));
		return;
	}

	KnobComponent->SetRange(FrequencyRange);
	KnobComponent->SetValue(StartFrequency);

	if (!ensure(ScreenComponent))
	{
		UE_LOG(LogTemp, Error,
		       TEXT("Missing Screen Component"));
		return;
	}

	if (AScreenComponent* ScreenActor = Cast<AScreenComponent>(ScreenComponent->GetChildActor()))
	{
		ScreenActor->SetRadio(this);
	}

	if (!ensure(WidgetComponent))
	{
		return;
	}

	const FText NewText = FText::FromString(FString::Printf(TEXT("%.0f Hz"), GetFrequency()));
	if (ensure(TextWidgetClass))
	{
		WidgetComponent->SetWidgetClass(TextWidgetClass);
		WidgetComponent->InitWidget();
		URadioText* RefRadioText = Cast<URadioText>(WidgetComponent->GetWidget());
		check(RefRadioText);
		RefRadioText->SetRadioText(NewText);
	}

	if (!ensure(DecoderComponent))
	{
		return;
	}

	if (ADecoder* DecoderActor = Cast<ADecoder>(DecoderComponent->GetChildActor()))
	{
		Decoder = DecoderActor;
	}

	ALukaController* LukaController = Cast<ALukaController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!ensure(LukaController))
	{
		return;
	}
	LukaController->OnUnpossessToyTransition.AddDynamic(Decoder, &ADecoder::OnEndToyPossessEvent);
}

void ARadio::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (bIsPossessed)
	{
		UpdateVisuals();
		UpdateStations();
	}

	if (bIsPossessed)
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController->WasInputKeyJustPressed(EKeys::Enter)) {
			bool bIsInRange = false;
			AStation *Station = GetNearestStation(bIsInRange);
			if (bIsInRange && Station)
			{
				Station->Skip(PlayerController->IsInputKeyDown(EKeys::LeftShift));
			}
		}
		if (PlayerController->WasInputKeyJustPressed(EKeys::U)) {
			Lock(false);
		}
	}
}

void ARadio::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (!ensure(EnhancedInputComponent))
	{
		return;
	}

	EnhancedInputComponent->BindAction(InputActionShiftFrequency, ETriggerEvent::Triggered, this,
	                                   &ARadio::ShiftFrequency);

	EnhancedInputComponent->BindAction(InputActionAnswer, ETriggerEvent::Triggered, this,
									   &ARadio::HandleAnswer);

	if (ensure(Decoder))
	{
		Decoder->SetupPlayerInputComponent(EnhancedInputComponent);
	}
}

void ARadio::Destroyed()
{
	StopRadio();

	for (AStation* Station : Stations)
	{
		if (Station)
		{
			Station->Destroy();
		}
	}

	Stations.Empty();
	StationsInRange.Empty();

	Super::Destroyed();
}

void ARadio::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (ALukaController* LukaController = Cast<ALukaController>(NewController))
	{
		StartRadio();
	}

	APlayerController* PlayerController = Cast<APlayerController>(NewController);
	if (!PlayerController)
	{
		return;
	}
}

void ARadio::UnPossessed()
{
	Super::UnPossessed();
	StopRadio();
}

AStation* ARadio::CreateStationObject(UStationAsset* StationData)
{
	check(StationData);
	check(SoundPosition);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;

	if (ensure(StationData->StationClass)) // Your Station blueprint seems to be missing a class
	{
		
		AStation* NewStation = GetWorld()->SpawnActor<AStation>(StationData->StationClass,
																FVector::ZeroVector, FRotator::ZeroRotator,
																SpawnParameters);

		check(NewStation);
		NewStation->AttachToComponent(SoundPosition,
									  FAttachmentTransformRules::KeepRelativeTransform);
		NewStation->SetupStation(StationData, this);
		Stations.Add(NewStation);
		return NewStation;
	}

	return nullptr;
}

AStation* ARadio::GetStationFromDataAsset(const UStationAsset* StationData)
{
	for (AStation* StationActor : Stations)
	{
		if (!ensure(StationActor)) { continue; }
		if (StationActor->GetStationData() == StationData)
		{
			return StationActor;
		}
	}
	return nullptr;
}

void ARadio::ForgetStationObject(AStation* Station)
{
	check(Station);

	Stations.Remove(Station);
	StationsInRange.Remove(Station);
}

void ARadio::StartRadio()
{
	// Start Noise
	if (!ensure(NoiseStartEvent))
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Missing 'Noise Start Event' in Radio class defaults — the radio will never start without it."));
		return;
	}
	if (!ensure(NoiseStopEvent))
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Missing 'Noise Stop Event' in Radio class defaults — the radio will never stop without it."));
		return;
	}
	check(RadioAkComponent);

	FOnAkPostEventCallback NullCallback;
	RadioAkComponent->PostAkEvent(NoiseStartEvent, 0, NullCallback);
}

void ARadio::StopRadio()
{
	for (auto It = StationsInRange.CreateIterator(); It; ++It)
	{
		AStation* Station = *It;
		if (!ensure(Station))
		{
			continue;
		}

		Station->LeaveRange();
		It.RemoveCurrent();
	}

	// Stop Noise
	if (!ensure(NoiseStartEvent))
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Missing 'Noise Start Event' in Radio class defaults — the radio will never start without it."));
		return;
	}
	if (!ensure(NoiseStopEvent))
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Missing 'Noise Stop Event' in Radio class defaults — the radio will never stop without it."));
		return;
	}

	check(RadioAkComponent);
	const FOnAkPostEventCallback NullCallback;
	RadioAkComponent->PostAkEvent(NoiseStopEvent, 0, NullCallback);

	if (Decoder)
	{
		Decoder->StopDecoder();
	}
}

float ARadio::GetRelativeStationPosition(AStation* Station)
{
	if (!ensure(Station))
	{
		return -10000000.f;
	}

	return Station->GetFrequency() - GetFrequency();
}

void ARadio::UpdateStations()
{
	float MaxStationClarity = 0.f;

	float Frequency = GetFrequency();

	for (TObjectPtr<AStation> Station : Stations)
	{
		if (!ensure(Station))
		{
			continue;
		}

		if (Station->IsInRange(Frequency))
		{
			if (!StationsInRange.Contains(Station))
			{
				StationsInRange.Add(Station);
				Station->EnterRange();
			}
		}
		else
		{
			if (StationsInRange.Contains(Station))
			{
				StationsInRange.Remove(Station);
				Station->LeaveRange();
			}
		}

		const float StationClarity = Station->ComputeRawClarity(Frequency);

		if (ensure(AudioCorruptionCurve))
		{
			Station->SetClarity(AudioCorruptionCurve->GetFloatValue(StationClarity), SubtitlesCorruptionCurve->GetFloatValue(StationClarity));
		} else
		{
			Station->SetClarity(StationClarity, StationClarity);
		}
		
		MaxStationClarity = FMath::Max(StationClarity, MaxStationClarity);
	}

	if (NoiseRtpc != nullptr)
	{
		RadioAkComponent->SetRTPCValue(NoiseRtpc, MaxStationClarity, 0, NoiseRtpc->GetName());
	}

	else UE_LOG(LogTemp, Warning,
	            TEXT("Missing 'Noise RTPC' in Radio class defaults — the noise volume will never change without it."));

	OnClarityUpdate(MaxStationClarity);
}

AStation *ARadio::GetNearestStation(bool &bIsInRange) const
{
	check(KnobComponent)
	float Frequency = GetFrequency();
	float Furthest = TNumericLimits<float>::Max();
	AStation *NearestStation = nullptr;
	for (AStation* Station : Stations)
	{
		float StationFrequencyDistance = FMath::Abs(Station->GetFrequency() - Frequency);
		if (StationFrequencyDistance < Furthest)
		{
			NearestStation = Station;
			Furthest = StationFrequencyDistance;
		}
	}

	if (NearestStation)
	{
		bIsInRange = NearestStation->IsInRange(Frequency);
	} else
	{
		bIsInRange = false;
	}

	return NearestStation;
}

TSet<TObjectPtr<class AStation>> const &ARadio::GetStationsInRange() const
{
	return StationsInRange;
}

void ARadio::Lock(const bool bEnable, AStation* Station)
{
	Super::Lock(bEnable);

	if (bEnable && Station)
	{
		GetWorldTimerManager().SetTimer(LockTimer, [this, Station]()
		{
			if (!Station)
			{
				GetWorldTimerManager().ClearTimer(LockTimer);
				LockTimer.Invalidate();
				return;
			}
			if (FMath::Abs(Station->GetFrequency() - GetFrequency()) < 0.1f)
			{
				GetWorldTimerManager().ClearTimer(LockTimer);
				LockTimer.Invalidate();
				return;
			}

			float CurrentFrequency = GetFrequency();
			float TargetFrequency = Station->GetFrequency();
			float Difference = TargetFrequency - CurrentFrequency;
			Difference *= LerpSpeed;
			KnobComponent->ClockwiseRotate(Difference, GetWorld()->GetDeltaSeconds());
		}, 0.01f, true);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(LockTimer);
	}
}

void ARadio::ShiftFrequency(const FInputActionValue& Value)
{
	check(KnobComponent);

	/* DEPRECATED
	if (FrequencyControlType == EFrequencyControlType::EFCT_Spin)
	{
		const FVector2d RecordedValue = Value.Get<FVector2D>();
		const float Length = RecordedValue.Length();
		const float Angle = UTLUtils::GetAngleFrom2DVector(RecordedValue);

		if (bIsNotFirstRollAngle)
		{
			if (Length < MinimumRollInputRegister)
			{
				bIsNotFirstRollAngle = false;
			}
			else
			{
				const float RelativeValue = UTLUtils::GetAngleDelta(Angle, LastAngle)) / 360.f;
				KnobComponent->ClockwiseRotate(RelativeValue);
				LastAngle = Angle;
			}
		}
		else
		{
			if (Length >= MinimumRollInputRegister)
			{
				bIsNotFirstRollAngle = true;
				LastAngle = Angle;
			}
		}
	}
	*/
	const float RelativeValue = Value.Get<float>();
	KnobComponent->ClockwiseRotate(RelativeValue, GetWorld()->GetDeltaSeconds());
}

void ARadio::HandleAnswer(const struct FInputActionValue& Value)
{
	for (AStation *Station : GetStationsInRange())
	{
		if (Station->ProposeAnswer()) // First one to be waiting for an answer gets it (later, probably better to pick the closest)
		{
			return;
		}
	}
}

ADecoder* ARadio::GetDecoder() const
{
	check(Decoder);
	return Decoder;
}

FVector2d ARadio::GetFrequencyRange() const
{
	return FrequencyRange;
}

float ARadio::GetFrequency() const
{
	if (ensure(KnobComponent))
	{
		return KnobComponent->GetValue();
	}
	
	return -1.f;
}

void ARadio::UpdateVisuals() const
{
	UpdateText();

	if (!ensure(ScreenComponent) || !ensure(KnobComponent))
	{
	}
}

void ARadio::UpdateText() const
{
	check(WidgetComponent);

	const FText NewText = FText::FromString(FString::Printf(TEXT("%.0f Hz"), GetFrequency()));
	if (UUserWidget const *Widget = WidgetComponent->GetWidget(); ensure(Widget))
	{
		const URadioText* RadioText = Cast<URadioText>(Widget);
		check(RadioText);
		RadioText->SetRadioText(NewText);
	}
}