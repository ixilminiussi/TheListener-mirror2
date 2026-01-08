#include "UI/Subtitles/SubtitleTextWidget.h"
#include "Components/RichTextBlock.h"
#include "Kismet/GameplayStatics.h"

void USubtitleTextWidget::SetSubtitleInfo(FSubtitleInfo const* InSubtitleInfo)
{
	check(InSubtitleInfo);
	SubtitleInfo = *InSubtitleInfo;
}

void USubtitleTextWidget::SetClarity(const float InClarity)
{
	Clarity = InClarity;
}

void USubtitleTextWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Clarity != 1.0f && SubtitleInfo.Source == ESource::ESRadio)
	{
		//Get a "Unique" Seed for each subtitle
		int CurrentSeed = SubtitleInfo.SubtitleEn.Len();
		const int Corruption = Clarity * 100;
		FString ModifiedText;
		bool NoCorrupt = false;
		for (int i = 0; i < SubtitleInfo.SubtitleEn.Len(); i++)
		{
			if (CurrentSeed == 0)
			{
				CurrentSeed = 10000;
			}
			
			CurrentSeed = SubtitleInfo.SubtitleEn[i] * CurrentSeed % 1000000;
			FMath::RandInit(CurrentSeed);

			constexpr char Space = 32;
			constexpr char RichTextTagIn = 60;
			constexpr char RichTextTagOut = 62;
			if (SubtitleInfo.SubtitleEn[i] == RichTextTagIn) {NoCorrupt = true;}
			if (SubtitleInfo.SubtitleEn[i] == RichTextTagOut) {NoCorrupt = false;}
			
			if (FMath::RandHelper(100) > Corruption && SubtitleInfo.SubtitleEn[i] != Space && !NoCorrupt)
			{
				FMath::RandInit(static_cast<int>((i + 1) * 10000 * UGameplayStatics::GetTimeSeconds(this)) % 100000);
				char rChar = FMath::RandRange(65, 116);
				if (rChar > 90) { rChar += 6; }
				ModifiedText.AppendChar(rChar);
			}
			else
			{
				ModifiedText.AppendChar(SubtitleInfo.SubtitleEn[i]);
			}
		}
		
		if (!ensure(SubtitleText)) { return; }
		SubtitleText->SetText(FText::FromString(ModifiedText));
	} else
	{
		if (!ensure(SubtitleText)) { return; }
		SubtitleText->SetText(FText::FromString(SubtitleInfo.SubtitleEn));
	}
}
