#include "UI/PlayWidget.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/Prompt/ActiveCommandWidget.h"
#include "UI/Prompt/HoverCommandWidget.h"
#include "UI/Subtitles/SubtitlesWidget.h"

UCursorWidget* UPlayWidget::GetCursorWidget() const
{
	check(CursorWidget);
	return CursorWidget;
}

USubtitlesWidget* UPlayWidget::GetSubtitlesWidget() const
{
	check(SubtitlesWidget);
	return SubtitlesWidget;
}

class UCanvasPanel* UPlayWidget::GetInspectablePanel()
{
	check(InspectCanvas)
	return InspectCanvas;
}

UAnswerWidget* UPlayWidget::GetAnswerWidget() const
{
	check(AnswerWidget);
	return AnswerWidget;
}

UPromptsHolder* UPlayWidget::GetPromptsHolder() const
{
	check(PromptsHolder);
	return PromptsHolder;
}

void UPlayWidget::AddHoverCommandWidget(UHoverCommandWidget* HoverCommandWidget) const
{
	check(HoverCommandWidget);
	check(VerticalBox);
	VerticalBox->AddChildToVerticalBox(HoverCommandWidget);
}

void UPlayWidget::RemoveHoverCommandWidget(UHoverCommandWidget* HoverCommandWidget) const
{
	check(HoverCommandWidget);
	check(VerticalBox);
	VerticalBox->RemoveChild(HoverCommandWidget);
}

void UPlayWidget::AddActiveCommandWidget(UActiveCommandWidget* ActiveCommandWidget) const
{
	check(ActiveCommandWidget);
	check(ActiveVerticalBox);

	// if (UVerticalBoxSlot* VerticalBoxSlot = ActiveVerticalBox->AddChildToVerticalBox(ActiveCommandWidget))
	// {
	// 	VerticalBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	// 	VerticalBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	// 	VerticalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Type::Fill));
	// 	VerticalBoxSlot->SetPadding(FMargin(10.0f));
	// }

	if (UVerticalBoxSlot* VerticalBoxSlot = ActiveVerticalBox->AddChildToVerticalBox(ActiveCommandWidget))
	{
		VerticalBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);
		VerticalBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Top);
		VerticalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Type::Fill));
	}
}

void UPlayWidget::RemoveActiveCommandWidget(UActiveCommandWidget* ActiveCommandWidget) const
{
	check(ActiveCommandWidget);
	check(ActiveVerticalBox);
	ActiveVerticalBox->RemoveChild(ActiveCommandWidget);
}