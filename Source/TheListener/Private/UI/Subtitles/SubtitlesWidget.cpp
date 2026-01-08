// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Subtitles/SubtitlesWidget.h"
#include "Components/VerticalBox.h"
#include "UI/Subtitles/SubtitleTextWidget.h"
#include "Components/VerticalBoxSlot.h"

int32 USubtitlesWidget::PushSubtitle(FName const& Tag)
{
	static int32 IDDispenser = -1;
	
	IDDispenser ++;

	USubtitleTextWidget* Subtitle = CreateWidget<USubtitleTextWidget>(GetOwningPlayer(), SubtitleTextClass);
	check(Subtitle);
	Subtitle->SetVisibility(ESlateVisibility::Collapsed);
	UVerticalBoxSlot* BoxSlot = SubtitleContainer->AddChildToVerticalBox(Subtitle);
	check(BoxSlot);
	BoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	BoxSlot->SetHorizontalAlignment(HAlign_Center);
	BoxSlot->SetVerticalAlignment(VAlign_Center);

	if (ensure(SubtitlesDataTable))
	{
		if (FSubtitleInfo *SubtitleInfo = SubtitlesDataTable->FindRow<FSubtitleInfo>(Tag, TEXT("Loading Subtitle")))
		{
			Subtitle->SetSubtitleInfo(SubtitleInfo);
		}
	}
	
	Subtitles.Add(IDDispenser, Subtitle);

	return IDDispenser;
}

void USubtitlesWidget::UpdateSubtitle(const int32 ID, FString const& Tag)
{
	check(ID != INVALID_ID)
	if (!ensure(Subtitles.Contains(ID)))
	{
		return;
	}
	
	if (ensure(SubtitlesDataTable))
	{
		const FSubtitleInfo *SubtitleInfo = SubtitlesDataTable->FindRow<FSubtitleInfo>(FName(Tag), TEXT("Loading Subtitle"));
		if (SubtitleInfo)
		{
			Subtitles[ID]->SetSubtitleInfo(SubtitleInfo);
		}
	}
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
	} else
	{
		Subtitles[ID]->SetVisibility(ESlateVisibility::Visible);
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
}
