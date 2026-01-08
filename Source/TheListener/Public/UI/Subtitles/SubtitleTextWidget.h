// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "AkGameplayStatics.h"
#include "SubtitleTextWidget.generated.h"

UENUM()
enum class ESource : uint8
{
	ESRadio,
	ESSpatial,
	ESPhone,
};

USTRUCT(BlueprintType)
struct FSubtitleInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Speaker;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SubtitleEn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SubtitleFr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESource Source;
};

/**
 * 
 */
UCLASS()
class THELISTENER_API USubtitleTextWidget : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	void SetSubtitleInfo(FSubtitleInfo const* InSubtitleInfo);
	void SetClarity(float InClarity);

	virtual void NativeTick(const FGeometry&, float InDeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class URichTextBlock* SubtitleText;
	
	FSubtitleInfo SubtitleInfo;

	float Clarity;

	UPROPERTY(EditDefaultsOnly, Category = "Subtitles")
	TObjectPtr<class UCurveFloat> CorruptionCurve;
	
	UPROPERTY(EditDefaultsOnly, Category = "Subtitles")
	float TimeToScrambleLetter = 0.025f;
	
	FTimerHandle ScrambleTimer;
};

