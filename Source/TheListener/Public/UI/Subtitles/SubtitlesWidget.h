// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SubtitlesWidget.generated.h"

#define INVALID_ID -1

/**
 * 
 */
UCLASS()
class THELISTENER_API USubtitlesWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	[[nodiscard]] int32 PushSubtitle(FName const &Tag = "");
	auto UpdateSubtitle(int32 ID, FString const& Tag = "") -> void;
	void UpdateSubtitle(int32 ID, float Clarity);
	void PopSubtitle(int32 ID);
	
protected:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<class UVerticalBox> SubtitleContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Subtitles")
	TSubclassOf<class USubtitleTextWidget> SubtitleTextClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Subtitles")
	TObjectPtr<class UDataTable> SubtitlesDataTable;

	TMap<uint32, TObjectPtr<class USubtitleTextWidget>> Subtitles;
};
