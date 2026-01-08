#pragma once

#include "BaseHUD.h"
#include "CoreMinimal.h"

#include "LukaHUD.generated.h"

enum class EInteractionsTypes : uint8;
#define TOP_LEVEL 100000

UCLASS()
class THELISTENER_API ALukaHUD : public ABaseHUD
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnBeginInteractiveInViewEvent();
	UFUNCTION()
	void OnEndInteractiveInViewEvent();
	UFUNCTION()
	void OnBeginToyPossessEvent();
	UFUNCTION()
	void OnEndToyPossessEvent();

	class USubtitlesWidget* GetSubtitlesWidget() const;
	class UAnswerWidget* GetAnswerWidget() const;
	class UCanvasPanel* GetInspectablePanel() const;
	class UPromptsHolder* GetPromptsHolder() const;

	void AddHoverCommandWidget(class UHoverCommandWidget* HoverCommandWidget) const;
	void RemoveHoverCommandWidget(class UHoverCommandWidget* HoverCommandWidget) const;
	void AddActiveCommandWidget(class UActiveCommandWidget* ActiveCommandWidget) const;
	void RemoveActiveCommandWidget(class UActiveCommandWidget* ActiveCommandWidget) const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "LukaHUD|Widget")
	TSubclassOf<class UCommonActivatableWidget> MainWidgetClass = nullptr;

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
