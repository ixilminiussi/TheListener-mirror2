#include "GPE/Radio/Station.h"
#include "AkComponent.h"
#include "GPE/Radio/Radio.h"
#include "GPE/Radio/StationDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Miscellaneous/TLUtils.h"
#include "EventSubsystem.h"
#include "GPE/Radio/Decoder.h"
#include "Player/LukaController.h"
#include "UI/Dialogue/AnswerDataAsset.h"
#include "System/Events/EventCondition.h"
#include "System/Frequency/FrequencySubsystem.h"
#include "UI/LukaHUD.h"
#include "UI/Subtitles/SubtitlesWidget.h"

AStation::AStation(): SubtitleClarity(0), AudioClarity(0)
{
}

void AStation::Destroyed()
{
	Super::Destroyed();

	if (SubtitlesID != INVALID_ID)
	{
		DestroySubtitles();
	}

	if (AkComponent)
	{
		AkComponent->Stop();
	}
}

void AStation::BeginPlay()
{
	Super::BeginPlay();
}

void AStation::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	auto FrequencySubsystem = GetWorld()->GetSubsystem<UFrequencySubsystem>();
	check(FrequencySubsystem);
	FrequencySubsystem->ForgetStation(this);

	check(AkComponent);
	AkComponent->Stop();
}

void AStation::StartStation()
{
	check(AkComponent);
	check(StationAsset);

	if (DialogueTree) // Use Answer Runtime
	{
		StartDialogue();
	}
	else // Use LEGACY / DEFAULT Runtime
	{
		AudioEvent = StationAsset->StartAudioEvent;
		if (ensure(AudioEvent)) // TODO: YOU ARE MISSING AN AUDIO EVENT (or a AnswerDataAsset)
		{
			PostAkEvent();
		}
	}
}

void AStation::StopStation()
{
	// send condition
	const FConditionKey Key = UStationFinishedCondition::GenerateKey(StationAsset);
	if (const auto EventSubsystem = GetWorld()->GetSubsystem<UEventSubsystem>())
	{
		EventSubsystem->SetConditionValue(Key, true);
	}

	check(AkComponent)
	AkComponent->Stop();

	Lock(false);

	Destroy();
}

void AStation::SetClarity(float const InSubtitleClarity, float const InAudioClarity)
{
	SubtitleClarity = FMath::Clamp(InSubtitleClarity, 0.0f, MaxClarity);
	AudioClarity = FMath::Clamp(InAudioClarity, 0.0f, MaxClarity);
	
	bIsHeard = AudioClarity > 0.f;

	check(StationAsset);

	if (!ensure(StationAsset->Rtpc))
	{
		UE_LOG(LogTemp, Warning,
			   TEXT("Missing 'RTPC' in Station data asset — the volume will never change without it."));
		return;
	}
		
	AkComponent->SetRTPCValue(StationAsset->Rtpc, AudioClarity, 0, StationAsset->Rtpc->GetName());
	UpdateSubtitlesClarity(SubtitleClarity);
}

float AStation::ComputeRawClarity(const float InFrequency) const
{
	float Clarity = 1.0f - (FMath::Abs(StationAsset->Frequency - InFrequency) / StationAsset->ReceptionBand);
	Clarity = FMath::Clamp(Clarity, 0.f, 1.f);

	return Clarity;
}

void AStation::SetupStation(UStationAsset* InStationAsset, ARadio* RadioActor)
{
	StationAsset = InStationAsset;
	Radio = RadioActor;

	if (StationAsset->DialogueTreeData)
	{
		SetupDialogue(StationAsset->DialogueTreeData, Radio);
	}
}

UStationAsset* AStation::GetStationData() const
{
	return StationAsset;
}

float AStation::GetFrequency() const
{
	if (ensure(StationAsset))
	{
		return StationAsset->Frequency;
	}
	
	return -1.0f;
}

void AStation::OnAnswerQuestion_Implementation()
{
	Lock(true);
}

void AStation::HandleMarker(FString const& Marker)
{
	Super::HandleMarker(Marker);
	
	FString const ModMarker = "MOD_";
	if (Marker.StartsWith(ModMarker)) // Then it has custom values
	{
		FString ChoppedMarker = Marker.Mid(ModMarker.Len());

		FString const MaxClarityMarker = "MAXCLAR_";
		if (ChoppedMarker.StartsWith(MaxClarityMarker))
		{
			MaxClarity = FCString::Atof(*(ChoppedMarker.RightChop(MaxClarityMarker.Len())));
			return;
		}
	}
}

void AStation::UpdateSubtitlesClarity(const float Clarity)
{
	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); PlayerController)
	{
		if (ALukaHUD* HUD = Cast<ALukaHUD>(PlayerController->GetHUD()); ensure(HUD))
		{
			HUD->GetSubtitlesWidget()->UpdateSubtitle(SubtitlesID, Clarity);
		}
	}
}

void AStation::OnEndDialogue_Implementation()
{
	StopStation();
}

void AStation::OnFinishDialogue()
{
	if (!DialogueTree)
	{
		StopStation();
		return;
	}

	Super::OnFinishDialogue();
}

int32 AStation::GetPlayPosition() const
{
	const int32 PlayPosition = UTLUtils::GetPlayPosition(PlayingID);

	return PlayPosition;
}

bool AStation::IsInRange(float InFrequency) const
{
	check(StationAsset)

	const float DistanceCurrentFrequency = FMath::Abs(StationAsset->Frequency - InFrequency);
	return DistanceCurrentFrequency <= StationAsset->ReceptionBand;
}

void AStation::EnterRange()
{
	const auto EventSubsystem = GetWorld()->GetSubsystem<UEventSubsystem>();
	check(EventSubsystem)

	const FConditionKey Key = UStationListenedCondition::GenerateKey(StationAsset);
	EventSubsystem->SetConditionValue(Key, true);

	if (QuestionID != INVALID_ID)
	{
		OpenPrompt();
	}
	
	OnEnteringStationRange();
}

void AStation::LeaveRange()
{
	if (!ensure(AkComponent) || !ensure(StationAsset))
	{
		return;
	}
	if (!ensure(StationAsset->Rtpc))
	{
		return;
	}
	
	AkComponent->SetRTPCValue(StationAsset->Rtpc, 0.f, 0, StationAsset->Rtpc->GetName());

	if (SubtitlesID != INVALID_ID)
	{
		UpdateSubtitlesClarity(0.f);
	}
	ClosePrompt();
	
	OnLeavingStationRange();
}

void AStation::Decode()
{
	OnDecoded();
}

bool AStation::IsHeard()
{
	return bIsHeard;
}

void AStation::StartDecoder()
{
	if (StationAsset->bIsEncoded)
	{
		ALukaController *LukaController = Cast<ALukaController>(Radio->GetController());
		check(LukaController)
		
		if (ADecoder *Decoder = Radio->GetDecoder(); ensure(Decoder))
		{
			Decoder->StartDecoder(LukaController, StationAsset->EncodeTargetA, StationAsset->EncodeTargetB, false);
			Decoder->OnFinish.AddDynamic(this, &AStation::Decode);
		}
	}
}

void AStation::StopDecoder()
{
	if (StationAsset->bIsEncoded)
	{
		ALukaController *LukaController = Cast<ALukaController>(Radio->GetController());
		check(LukaController)
		ADecoder *Decoder = Radio->GetDecoder();
		if (ensure(Decoder))
		{
			Decoder->StopDecoder();
			Decoder->OnFinish.RemoveDynamic(this, &AStation::Decode);
		}
	}
}

void AStation::OnDecoded_Implementation()
{
	check(Radio)
}

void AStation::OnEnteringStationRange_Implementation()
{
	check(StationAsset)
	check(Radio)

	StartDecoder();
}

void AStation::OnLeavingStationRange_Implementation()
{
}