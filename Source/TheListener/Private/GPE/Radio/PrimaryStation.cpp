#include "GPE/Radio/PrimaryStation.h"


/*
void APrimaryStation::BeginPlay()
{
	Super::BeginPlay();
}

void APrimaryStation::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsInRange)
	{
		UpdateText();
	}
}

void APrimaryStation::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	IsGameClosing = true;
	WasTrue = false;
}

void APrimaryStation::SetupStation(class UStationAsset* StationDataAsset, class ARadio* RadioActor)
{
	Super::SetupStation(StationDataAsset, RadioActor);
}

float APrimaryStation::GetCurrentInterest(const float CurrentFrequency) const
{
	if (!StationData->Rtpc)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Missing 'RTPC' in Station data asset — the volume will never change without it."));
		return .0f;
	}
	const float DistanceCurrentFrequency = FMath::Abs(StationData->Frequency - CurrentFrequency);
	if (DistanceCurrentFrequency <= StationData->ReceptionBand)
	{
		const float RtpcValue = 1.0f - (DistanceCurrentFrequency / StationData->ReceptionBand);
		return RtpcValue;
	}
	return .0f;
}

void APrimaryStation::UpdateText() const
{
	if (!ensure(SubtitlesWidget))
	{
		return;
	}
	if (SubtitlesWidget && LastPureLine != "")
	{
		SubtitlesWidget->SetText(LastPureLine);
	}
}

void APrimaryStation::LaunchStation()
{
	ALukaHUD* HUD = Cast<ALukaHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	if (ensure(HUD))
	{
		SubtitlesWidget = HUD->GetSubtitleWidget()->CreateSubtitle();
	}

	if (!ensure(SubtitlesWidget))
	{
		return;
	}
	SubtitlesWidget->SetVisibility(ESlateVisibility::Visible);


	if (!ensure(AkComponent))
	{
		return;
	}
	if (!ensure(StationData))
	{
		return;
	}

	OnStartStation();
	PostAkEvent();
	AkComponent->SetRTPCValue(StationData->Rtpc, 1.f, 0, StationData->Rtpc->GetName());
	AkComponent->SetRTPCValue(Radio->NoiseRtpc, 0.f, 0, StationData->Rtpc->GetName());

	bIsInRange = true;
	Radio->Lock();

	ALukaController* LukaController = Cast<ALukaController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!ensure(LukaController))
	{
		return;
	}
	LukaController->SetGlobalInputsStatus(false);
}

void APrimaryStation::OnAkEventCallback(EAkCallbackType CallbackType, UAkCallbackInfo* CallbackInfo)
{
	Super::OnAkEventCallback(CallbackType, CallbackInfo);
	switch (CallbackType)
	{
	case EAkCallbackType::Marker:
	case EAkCallbackType::MusicSyncUserCue:
		UpdateSubtitles(CallbackType, CallbackInfo);
		break;
	case EAkCallbackType::EndOfEvent:
		if (!IsGameClosing && !StationData->CanAnswer)
		{
			OnStationFinished();
		}
		break;
	default:
		break;
	}
}

void APrimaryStation::OnStationFinished()
{
	if (IsGameClosing)
	{
		return;
	}

	bIsInRange = false;
	WasTrue = false;
	check(Radio);
	Radio->Unlock();

	Radio->ForgetStationObject(this);
	Destroy();

	check(Decoder);
	Decoder->StopDecoder();

	const ALukaController* LukaController = Cast<ALukaController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!ensure(LukaController))
	{
		return;
	}
	LukaController->SetGlobalInputsStatus(true);

	const FConditionKey Key = UStationListenedCondition::GenerateKey(StationData);

	const auto EventSubsystem = GetWorld()->GetSubsystem<UEventSubsystem>();
	check(EventSubsystem);
	EventSubsystem->SetConditionValue(Key, true);
}

void APrimaryStation::SetDecoder(class ADecoder* DecoderActor)
{
	Decoder = DecoderActor;
}
*/
