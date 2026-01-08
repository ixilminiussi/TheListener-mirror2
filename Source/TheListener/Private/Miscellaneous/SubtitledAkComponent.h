// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AkComponent.h"
#include "SubtitledAkComponent.generated.h"

/**
 * 
 */
UCLASS()
class USubtitledAkComponent : public UAkComponent
{
	GENERATED_BODY()
	
	public:
	UFUNCTION(BlueprintCallable)
	void PostSubtitledEvent(class UAkAudioEvent* AkEvent);
	
	protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void CreateSubtitles();
	void DestroySubtitles();
	int32 SubtitlesID;

	UFUNCTION()
    void OnAkEventCallback(EAkCallbackType CallbackType, UAkCallbackInfo* CallbackInfo);
	void HandleMarker(FString const& Marker);
};
