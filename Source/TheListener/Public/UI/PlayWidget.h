#pragma once
#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PlayWidget.generated.h"

UCLASS()
class THELISTENER_API UPlayWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	class UCursorWidget* GetCursorWidget() const;
	class UCanvasPanel* GetInspectablePanel() const;
	class USubtitlesWidget* GetSubtitlesWidget() const;
	class UAnswerWidget* GetAnswerWidget() const;
	class UPromptsHolder* GetPromptsHolder() const;

	UFUNCTION(BlueprintNativeEvent)
	void NotifyClue();
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCursorWidget> CursorWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USubtitlesWidget> SubtitlesWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> InspectCanvas;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UAnswerWidget> AnswerWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UPromptsHolder> PromptsHolder;
};