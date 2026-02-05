#pragma once

#include "BaseHUD.h"
#include "CoreMinimal.h"
#include "LukaHUD.generated.h"

enum class EInteractionsTypes : uint8;
#define TOP_LEVEL 100000

DECLARE_DELEGATE_OneParam(FInteractiveInView, bool);
DECLARE_DELEGATE(FOnShowCursor);
DECLARE_DELEGATE(FOnHideCursor);

UCLASS()
class THELISTENER_API ALukaHUD : public ABaseHUD
{
	GENERATED_BODY()

public:
	static FInteractiveInView InteractiveInView;
	static FOnShowCursor OnShowCursor;
	static FOnHideCursor OnHideCursor;

	void ToggleCursorVisibility(bool bVisible) const;
	class USubtitlesWidget* GetSubtitlesWidget() const;
	class UAnswerWidget* GetAnswerWidget() const;
	class UCanvasPanel* GetInspectablePanel() const;
	class UPromptsHolder* GetPromptsHolder() const;
	void NotifyClue() const;

protected:
	UFUNCTION()
	void InViewUpdate(bool bToggle);
	bool bInView;
	
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "LukaHUD|Widget")
	TSubclassOf<class UPlayWidget> PlayWidgetClass = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UPlayWidget> PlayWidget = nullptr;

	UPROPERTY()
	TObjectPtr<class UCommonActivatableWidget> EndPanelWidgetInstance = nullptr;

	virtual UBaseMenuWidget* GetPreviousWidget() override;

	virtual USettingsMenuWidget* GetSettingsMenuWidget() const override;

	UFUNCTION(BlueprintCallable)
	class UCommonActivatableWidget* GetEndPanelWidget() const;

public:
	UFUNCTION()
	void PauseGame();
	UFUNCTION()
	void ResumeGame();

	UFUNCTION(BlueprintImplementableEvent)
	void PauseGameInBlueprint();
	UFUNCTION(BlueprintImplementableEvent)
	void ResumeGameInBlueprint();

	UFUNCTION(BlueprintNativeEvent)
	void HandleAnswerInput() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="LukaHUD|Pause")
	TObjectPtr<class UAkAudioEvent> PauseEvent;
	
	UPROPERTY(EditDefaultsOnly, Category="LukaHUD|Pause")
	TObjectPtr<class UAkAudioEvent> ResumeEvent;
	
	UPROPERTY(EditDefaultsOnly, Category="LukaHUD|Enhanced Inputs")
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, Category = "Widget|Menus")
	TSubclassOf<class UCommonUserWidget> EndPanelWidgetClass;
};
