#include "UI/Subtitles/SubtitleTextWidget.h"
#include "Components/RichTextBlock.h"
#include "Kismet/GameplayStatics.h"

void USubtitleTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StyleTable = SubtitleText->GetTextStyleSet();
}

void USubtitleTextWidget::SetSubtitleInfo(const FSubtitleInfo* InSubtitleInfo)
{
	check(InSubtitleInfo);
	SubtitleInfo = *InSubtitleInfo;
	CurrentStyle = "DefaultName";

	if (FRichTextStyleRow* StyleRow = StyleTable->FindRow<FRichTextStyleRow>(FName(*SubtitleInfo.Speaker), ""))
	{
		CurrentStyle = SubtitleInfo.Speaker;
	}
}

void USubtitleTextWidget::SetClarity(const float InClarity)
{
	Clarity = InClarity;
}

void USubtitleTextWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (SubtitleInfo.SubtitleEn.Len() == 0)
	{
		SubtitleText->SetText({});
		return;
	}


	FString CurrentLine;
	CurrentLine.Append("<");
	CurrentLine.Append(CurrentStyle);
	CurrentLine.Append(">");
	CurrentLine.Append(SubtitleInfo.Speaker);
	CurrentLine.Append("</>\n");
	bool IsEnglish = false;
	CurrentLine.Append(IsEnglish ? SubtitleInfo.SubtitleEn : SubtitleInfo.SubtitleFr);
	if (Clarity != 1.0f && SubtitleInfo.Source == ESource::ESRadio)
	{
		//Get a "Unique" Seed for each subtitle
		int CurrentSeed = CurrentLine.Len();
		const int Corruption = Clarity * 100;
		FString ModifiedText;
		bool NoCorrupt = false;
		for (int i = 0; i < CurrentLine.Len(); i++)
		{
			constexpr char Space = 32;
			constexpr char RichTextTagIn = 60;
			constexpr char RichTextTagOut = 62;
			constexpr char Colon = 58;
			if (CurrentLine[i] == RichTextTagIn) { NoCorrupt = true; }
			if (CurrentLine[i] == RichTextTagOut) { NoCorrupt = false; }

			if ((CurrentLine[i] == Colon || CurrentLine[i] == Space) || NoCorrupt)
			{
				ModifiedText.AppendChar(CurrentLine[i]);
				continue;
			}

			CurrentSeed = CurrentLine[i] * CurrentSeed % 1000000;
			FMath::RandInit(CurrentSeed);

			if (FMath::RandHelper(100) > Corruption && CurrentLine[i] != Space)
			{
				FMath::RandInit(static_cast<int>((i + 1) * 10000 * UGameplayStatics::GetTimeSeconds(this)) % 100000);
				char rChar = FMath::RandRange(65, 116);
				if (rChar > 90) { rChar += 6; }
				ModifiedText.AppendChar(rChar);
			}
			else
			{
				ModifiedText.AppendChar(CurrentLine[i]);
			}
		}

		if (!ensure(SubtitleText)) { return; }
		SubtitleText->SetText(FText::FromString(ModifiedText));
	}
	else
	{
		if (!ensure(SubtitleText)) { return; }
		SubtitleText->SetText(FText::FromString(CurrentLine));
	}
}

void USubtitleTextWidget::SetIsEnglish(bool InIsEnglish)
{
	bIsEnglish = InIsEnglish;
}

bool USubtitleTextWidget::GetIsEnglish() const
{
	return bIsEnglish;
}
