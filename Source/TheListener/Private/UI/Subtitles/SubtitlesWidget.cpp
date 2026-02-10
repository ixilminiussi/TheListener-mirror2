// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Subtitles/SubtitlesWidget.h"

#include "IMediaTracks.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/VerticalBox.h"
#include "UI/Subtitles/SubtitleTextWidget.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/Menus/UIElements/DropdownSettings.h"

int32 USubtitlesWidget::PushSubtitle(const FName& Tag)
{
	static int32 IDDispenser = -1;

	IDDispenser++;

	SubtitleBackground->SetVisibility(ESlateVisibility::Visible);

	USubtitleTextWidget* Subtitle = CreateWidget<USubtitleTextWidget>(GetOwningPlayer(), SubtitleTextClass);
	check(Subtitle);
	Subtitle->SetIsEnglish(bIsEnglish);
	Subtitle->SetVisibility(ESlateVisibility::Collapsed);
	UVerticalBoxSlot* BoxSlot = SubtitleContainer->AddChildToVerticalBox(Subtitle);
	check(BoxSlot);
	BoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	BoxSlot->SetHorizontalAlignment(HAlign_Center);
	BoxSlot->SetVerticalAlignment(VAlign_Center);

	if (ensure(SubtitlesDataTable))
	{
		if (FSubtitleInfo* SubtitleInfo = SubtitlesDataTable->FindRow<FSubtitleInfo>(Tag, TEXT("Loading Subtitle")))
		{
			Subtitle->SetSubtitleInfo(SubtitleInfo);
		}
	}

	Subtitles.Add(IDDispenser, Subtitle);

	return IDDispenser;
}

void USubtitlesWidget::UpdateSubtitle(const int32 ID, const FString& Tag)
{
	check(ID != INVALID_ID)
	if (!ensure(Subtitles.Contains(ID)))
	{
		return;
	}

	if (ensure(SubtitlesDataTable))
	{
		const FSubtitleInfo* SubtitleInfo = SubtitlesDataTable->FindRow<FSubtitleInfo>(
			FName(Tag), TEXT("Loading Subtitle"));
		if (SubtitleInfo)
		{
			if (SubtitleInfo->SubtitleEn.Len() > 0)
			{
				Subtitles[ID]->SetVisibility(ESlateVisibility::Visible);
				SubtitleBackground->SetVisibility(ESlateVisibility::Visible);

				Subtitles[ID]->SetSubtitleInfo(SubtitleInfo);
				return;
			}
		}
	}
	Subtitles[ID]->SetVisibility(ESlateVisibility::Collapsed);
	SubtitleBackground->SetVisibility(ESlateVisibility::Collapsed);
}

void USubtitlesWidget::UpdateSubtitle(const int32 ID, const float Clarity)
{
	check(ID != INVALID_ID)
	if (!ensure(Subtitles.Contains(ID)))
	{
		return;
	}

	Subtitles[ID]->SetClarity(Clarity);

	if (Clarity == 0.f)
	{
		Subtitles[ID]->SetVisibility(ESlateVisibility::Collapsed);
		SubtitleBackground->SetVisibility(ESlateVisibility::Collapsed);

	}
	else
	{
		Subtitles[ID]->SetVisibility(ESlateVisibility::Visible);
		SubtitleBackground->SetVisibility(ESlateVisibility::Visible);

	}
}

void USubtitlesWidget::PopSubtitle(int32 ID)
{
	check(ID != INVALID_ID)
	if (!ensure(Subtitles.Contains(ID)))
	{
		return;
	}

	Subtitles[ID]->RemoveFromParent();
	Subtitles.Remove(ID);
	SubtitleBackground->SetVisibility(ESlateVisibility::Collapsed);
}

void USubtitlesWidget::SetBakcgroundOpacity(float Value)
{
	FLinearColor BackgroundOpacity = FLinearColor(SubtitleBackground->GetColorAndOpacity().R,
	                                              SubtitleBackground->GetColorAndOpacity().G,
	                                              SubtitleBackground->GetColorAndOpacity().B, Value);
	SubtitleBackground->SetColorAndOpacity(BackgroundOpacity);
}

void USubtitlesWidget::SetSubtitlesTextStyle(float Value)
{
	if (SubtitlesTextStyle)
	{
		for (auto& Pair : SubtitlesTextStyle->GetRowMap())

		{
			FRichTextStyleRow* Row = reinterpret_cast<FRichTextStyleRow*>(Pair.Value);

			if (Row)
			{
				Row->TextStyle.Font.Size = FMath::RoundToInt(Row->TextStyle.Font.Size * Value);
			}
		}
	}
}

void USubtitlesWidget::SetLanguageBoolean(int32 Index)
{
	if (Index == 0)
	{
		bIsEnglish = true;
	}
	else
	{
		bIsEnglish = false;
	}
}
