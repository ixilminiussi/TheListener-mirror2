#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CursorWidget.generated.h"

UCLASS()
class THELISTENER_API UCursorWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetCursorVisibility(bool bIsVisible);
	void OnEnterPossibleInteraction();
	void OnExitPossibleInteraction();
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* CursorImage = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* OnEnterPossibleInteractionAnim = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* OnExitPossibleInteractionAnim = nullptr;
};