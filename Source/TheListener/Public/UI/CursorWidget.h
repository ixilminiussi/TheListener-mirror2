#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonUserWidget.h"
#include "CursorWidget.generated.h"

UCLASS()
class THELISTENER_API UCursorWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Grow();
	void Shrink();
	class USizeBox* GetCursorSizeBox() const;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* CursorImage = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USizeBox* CursorSizeBox = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* OnEnterPossibleInteractionAnim = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* OnExitPossibleInteractionAnim = nullptr;
};
