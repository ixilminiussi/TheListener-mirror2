#include "UI/PlayWidget.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/VerticalBoxSlot.h"

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

class UCanvasPanel* UPlayWidget::GetInspectablePanel() const
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

void UPlayWidget::NotifyClue_Implementation()
{
}
