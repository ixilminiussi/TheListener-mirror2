#include "Miscellaneous/TLUtils.h"

#include "AkGameplayStatics.h"
#include "AI/NavigationSystemBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/LukaHUD.h"
#include "UI/Prompt/PromptsHolder.h"
#include "Wwise/API/WwiseMusicEngineAPI.h"
#include "Wwise/API/WwiseSoundEngineAPI.h"

class ALukaHUD;

float UTLUtils::GetAngleFrom2DVector(const FVector2D Vector)
{
	float Value = Vector.X + FMath::Sqrt(Vector.X * Vector.X + Vector.Y * Vector.Y);
	if (Value != 0)
	{
		Value = Vector.Y / Value;
	}
	Value = FMath::Atan(Value);
	return (2.0f * FMath::RadiansToDegrees(Value));
}

float UTLUtils::GetAngleDelta(const float CurrentAngle, const float LastAngle)
{
	const float CurrentAngleRadian = FMath::DegreesToRadians(CurrentAngle);
	const float LastAngleRadian = FMath::DegreesToRadians(LastAngle);
	return ((FMath::Sin(LastAngleRadian) * FMath::Cos(CurrentAngleRadian)) - (FMath::Cos(LastAngleRadian) * FMath::Sin(
		CurrentAngleRadian)));
}

int32 UTLUtils::GetPlayPosition(const AkPlayingID PlayingID)
{
	//if (!ensureAlways(PlayingID != AK_INVALID_PLAYING_ID))
	//{
	if (IWwiseSoundEngineAPI* SoundEngine = IWwiseSoundEngineAPI::Get())
	{
		AkTimeMs TimeMs;
		if (const AKRESULT Result = SoundEngine->GetSourcePlayPosition(PlayingID, &TimeMs); Result == AK_Success)
		{
			return TimeMs;
		}
	}
	if (IWwiseMusicEngineAPI* MusicEngine = IWwiseMusicEngineAPI::Get())
	{
		AkSegmentInfo SegmentInfo;
		if (const AKRESULT Result = MusicEngine->GetPlayingSegmentInfo(PlayingID, SegmentInfo); Result ==
			AK_Success)
		{
			return SegmentInfo.iCurrentPosition;
		}
	}
	//}

	return -1;
}

TArray<AActor*> UTLUtils::GetAllActorsOfClass(UObject* WorldContextObject, TSubclassOf<AActor> Class)
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, Class, OutActors);
	return OutActors;
}

void UTLUtils::TogglePrompt(UObject* WorldContextObject, FName const& PromptName, bool bToggle)
{
	if (!ensure(WorldContextObject))
	{
		return;
	}
	if (const APlayerController *PlayerController = WorldContextObject->GetWorld()->GetFirstPlayerController(); ensure(PlayerController))
	{
		if (const ALukaHUD* LukaHUD = Cast<ALukaHUD>(PlayerController->GetHUD()); LukaHUD)
		{
			UPromptsHolder *PromptsHolder = LukaHUD->GetPromptsHolder();
			if (bToggle)
			{
				PromptsHolder->Show(PromptName);
			} else
			{
				PromptsHolder->Hide(PromptName);
			}
		}
	}
}

UWidget * UTLUtils::GetPrompt(UObject* WorldContextObject, FName const& PromptName)
{
	if (!ensure(WorldContextObject))
	{
		return nullptr;
	}
	if (const APlayerController *PlayerController = WorldContextObject->GetWorld()->GetFirstPlayerController(); ensure(PlayerController))
	{
		if (const ALukaHUD* LukaHUD = Cast<ALukaHUD>(PlayerController->GetHUD()); LukaHUD)
		{
			UPromptsHolder *PromptsHolder = LukaHUD->GetPromptsHolder();
			if (PromptsHolder)
			{
				return PromptsHolder->GetWidgetFromName(PromptName);
			}
		}
	}

	return nullptr;
}

void UTLUtils::OpenLevel(UObject* WorldContextObject, FName const& LevelName)
{
	if (!ensure(WorldContextObject))
	{
		return;
	}
	UGameplayStatics::OpenLevel(WorldContextObject->GetWorld(), LevelName);
}
