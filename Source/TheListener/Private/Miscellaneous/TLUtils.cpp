#include "Miscellaneous/TLUtils.h"

#include "AkGameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "AI/NavigationSystemBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/LukaHUD.h"
#include "UI/Prompt/PromptsHolder.h"
#include "Wwise/API/WwiseMusicEngineAPI.h"
#include "Wwise/API/WwiseSoundEngineAPI.h"
#include "EnhancedInput/Public/EnhancedActionKeyMapping.h"
#include "System/Frequency/FrequencySubsystem.h"

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

TArray<AActor*> UTLUtils::GetAllActorsOfClass(UObject const *WorldContextObject, TSubclassOf<AActor> Class)
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, Class, OutActors);
	return OutActors;
}

void UTLUtils::TogglePrompts(UObject* WorldContextObject, TArray<FName> const &PromptNames, bool bToggle)
{
	if (!ensure(WorldContextObject))
	{
		return;
	}
	if (bToggle)
	{
		ULocalPlayer* LocalPlayer = WorldContextObject->GetWorld()->GetFirstLocalPlayerFromController();
		const UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
		const TArray<FEnhancedActionKeyMapping> KeyMappings = EnhancedInputSubsystem->GetAllPlayerMappableActionKeyMappings();
		for (FName const &Name : PromptNames)
		{
			UPromptsHolder::ShowPrompt.Broadcast(Name, KeyMappings);
		}
	} else
	{
		for (FName const &Name : PromptNames)
		{
			UPromptsHolder::HidePrompt.Broadcast(Name);
		}
	}
}

void UTLUtils::OpenLevel(UObject* WorldContextObject, FName const& LevelName)
{
	if (!ensure(WorldContextObject))
	{
		return;
	}
	UGameplayStatics::OpenLevel(WorldContextObject->GetWorld(), LevelName);
}

/**
 * 
 * @param Start The place we start the step from
 * @param RelativeStep What the step would be at 1000 
 * @return What the new location is after the step
 */
float UTLUtils::FrequencyStep(const float Start, const float RelativeStep)
{
	const float Ratio = (1000.f + FMath::Abs(RelativeStep)) / 1000.f;

	if (RelativeStep < 0.0f)
	{
		return Start / Ratio;
	} else
	{
		return Start * Ratio;
	}
}

void UTLUtils::AllowBand(UObject* WorldContextObject, const int InBand)
{
	if (!ensure(WorldContextObject))
	{
		return;
	}
	UFrequencySubsystem *FrequencySubsystem = WorldContextObject->GetWorld()->GetSubsystem<UFrequencySubsystem>();
	if (ensure(FrequencySubsystem))
	{
		FrequencySubsystem->AllowBand(InBand);
	}
}
