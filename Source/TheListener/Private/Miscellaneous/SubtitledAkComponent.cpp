// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "Miscellaneous/SubtitledAkComponent.h"

#include "UI/LukaHUD.h"
#include "UI/Subtitles/SubtitlesWidget.h"

void USubtitledAkComponent::PostSubtitledEvent(UAkAudioEvent* Event)
{
	if (SubtitlesID != INVALID_ID)
	{
		DestroySubtitles();
	}
	
	CreateSubtitles();
	
	const FName CBName("OnAkEventCallback");
	FOnAkPostEventCallback AkPostEventCallback;
	AkPostEventCallback.BindUFunction(this, CBName);
	PostAkEvent(Event, AK_Marker | AK_MusicSyncUserCue | AK_EndOfEvent | AK_EnableGetMusicPlayPosition | AK_EnableGetSourcePlayPosition, AkPostEventCallback);
}

void USubtitledAkComponent::BeginPlay()
{
	SubtitlesID = INVALID_ID;
	
	Super::BeginPlay();
}

void USubtitledAkComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroySubtitles();
	
	Super::EndPlay(EndPlayReason);
}

void USubtitledAkComponent::CreateSubtitles()
{
	if (const APlayerController *PlayerController = GetWorld()->GetFirstPlayerController(); ensure(PlayerController))
	{
		if (const ALukaHUD* LukaHUD = Cast<ALukaHUD>(PlayerController->GetHUD()); LukaHUD)
		{
			USubtitlesWidget *SubtitlesWidget = LukaHUD->GetSubtitlesWidget();

			SubtitlesID = SubtitlesWidget->PushSubtitle();
		}
	}
}

void USubtitledAkComponent::DestroySubtitles()
{
	if (SubtitlesID == INVALID_ID)
	{
		return;
	}
	if (const APlayerController *PlayerController = GetWorld()->GetFirstPlayerController(); PlayerController)
	{
		if (const ALukaHUD* LukaHUD = Cast<ALukaHUD>(PlayerController->GetHUD()); LukaHUD)
		{
			USubtitlesWidget *SubtitlesWidget = LukaHUD->GetSubtitlesWidget();

			SubtitlesWidget->PopSubtitle(SubtitlesID);
		}
	}
}

void USubtitledAkComponent::OnAkEventCallback(EAkCallbackType CallbackType, UAkCallbackInfo* CallbackInfo)
{
	switch (CallbackType)
	{
	case EAkCallbackType::Marker:
		HandleMarker(FString(Cast<UAkMarkerCallbackInfo>(CallbackInfo)->Label));
		break;
	case EAkCallbackType::MusicSyncUserCue:
		HandleMarker(FString(Cast<UAkMusicSyncCallbackInfo>(CallbackInfo)->UserCueName));
		break;
	case EAkCallbackType::EndOfEvent:
		DestroySubtitles();
		break;
	default:
		break;
	}
}

void USubtitledAkComponent::HandleMarker(FString const& Marker)
{
	FString const ModMarker = "MOD_";
	if (Marker.StartsWith(ModMarker)) // Then it has custom values
	{
		FString ChoppedMarker = Marker.Mid(ModMarker.Len());

		FString const EarlyAnswerMarker = "STOP";
		if (ChoppedMarker.StartsWith(EarlyAnswerMarker))
		{
			if (const APlayerController *PlayerController = GetWorld()->GetFirstPlayerController(); PlayerController)
			{
				if (const ALukaHUD* LukaHUD = Cast<ALukaHUD>(PlayerController->GetHUD()); LukaHUD)
				{
					USubtitlesWidget *SubtitlesWidget = LukaHUD->GetSubtitlesWidget();

					SubtitlesWidget->UpdateSubtitle(SubtitlesID, "");
				}
			}
			return;
		}

		return;
	}
	
	if (const APlayerController *PlayerController = GetWorld()->GetFirstPlayerController(); PlayerController)
	{
		if (const ALukaHUD* LukaHUD = Cast<ALukaHUD>(PlayerController->GetHUD()); LukaHUD)
		{
			USubtitlesWidget *SubtitlesWidget = LukaHUD->GetSubtitlesWidget();

			SubtitlesWidget->UpdateSubtitle(SubtitlesID, Marker);
			SubtitlesWidget->UpdateSubtitle(SubtitlesID, 1.0f);
		}
	}
}
