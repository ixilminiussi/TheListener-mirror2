#include "UI/CursorWidget.h"
#include "Components/Image.h"

void UCursorWidget::Grow()
{
	StopAllAnimations();
	if(!ensure(OnEnterPossibleInteractionAnim != nullptr)) return;
	PlayAnimation(OnEnterPossibleInteractionAnim);
}

void UCursorWidget::Shrink()
{
	StopAllAnimations();
	if(!ensure(OnExitPossibleInteractionAnim != nullptr)) return;
	PlayAnimation(OnExitPossibleInteractionAnim);
}
