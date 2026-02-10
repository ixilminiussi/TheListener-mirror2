// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Menus/UIElements/DropdownSettings.h"
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
	[[nodiscard]] int32 PushSubtitle(const FName& Tag = "");
	void UpdateSubtitle(int32 ID, const FString& Tag = "");
	void UpdateSubtitle(int32 ID, float Clarity);
	void PopSubtitle(int32 ID);

	void SetBakcgroundOpacity(float Value);
	void SetSubtitlesTextStyle(float Value);
	void SetLanguageBoolean(int32 Index);

protected:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<class UVerticalBox> SubtitleContainer;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UImage* SubtitleBackground;

	UPROPERTY(EditDefaultsOnly, Category = "Subtitles")
	TSubclassOf<class USubtitleTextWidget> SubtitleTextClass;

	UPROPERTY(EditDefaultsOnly, Category = "Subtitles")
	TObjectPtr<class UDataTable> SubtitlesDataTable;


	UPROPERTY(EditDefaultsOnly, Category = "Subtitles")
	TObjectPtr<class UDataTable> SubtitlesTextStyle;

	TMap<uint32, TObjectPtr<class USubtitleTextWidget>> Subtitles;

	UPROPERTY(EditDefaultsOnly, Category = "Langage")
	bool bIsEnglish;
};
