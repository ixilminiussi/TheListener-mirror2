#pragma once

#include <AK/SoundEngine/Common/AkTypes.h>

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TLUtils.generated.h"

UCLASS()
class THELISTENER_API UTLUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Fills Destination array with tiles in a diamond shape. */
	UFUNCTION(BlueprintCallable, Category = Maths)
	static float GetAngleFrom2DVector(const FVector2D Vector);

	UFUNCTION(BlueprintCallable, Category = Maths)
	static float GetAngleDelta(const float CurrentAngle, const float LastAngle);

	static int32 GetPlayPosition(const AkPlayingID PlayingID);

	UFUNCTION(BlueprintCallable, Category = Statics)
	static TArray<AActor*> GetAllActorsOfClass(UObject* WorldContextObject, TSubclassOf<AActor> Class);

	UFUNCTION(BlueprintCallable, Category = Statics)
	static void TogglePrompt(UObject* WorldContextObject, FName const& PromptName, bool bToggle);

	UFUNCTION(BlueprintCallable, Category = Statics)
	static UWidget * GetPrompt(UObject* WorldContextObject, FName const& PromptName);

	UFUNCTION(BlueprintCallable, Category = Statics)
	static void OpenLevel(UObject* WorldContextObject, FName const &LevelName);
};
