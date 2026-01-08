#include "UI/CursorWidget.h"
#include "Components/Image.h"

void UCursorWidget::OnEnterPossibleInteraction()
{
	StopAllAnimations();
	if(!ensure(OnEnterPossibleInteractionAnim != nullptr)) return;
	PlayAnimation(OnEnterPossibleInteractionAnim);
}

void UCursorWidget::OnExitPossibleInteraction()
{
	StopAllAnimations();
	if(!ensure(OnExitPossibleInteractionAnim != nullptr)) return;
	PlayAnimation(OnExitPossibleInteractionAnim);
}

void UCursorWidget::SetCursorVisibility(bool bIsVisible)
{
	StopAllAnimations();
	if(!ensure(CursorImage != nullptr)) return;

	if(bIsVisible)
	{
		CursorImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CursorImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}