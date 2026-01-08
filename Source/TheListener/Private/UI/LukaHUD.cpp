#include "UI/LukaHUD.h"

#include "AkGameplayStatics.h"
#include "CommonActivatableWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LukaCharacter.h"
#include "Player/LukaController.h"
#include "UI/CursorWidget.h"
#include "UI/PlayWidget.h"
#include "UI/Menus/PauseMenuWidget.h"
#include "UI/Menus/SettingsMenuWidget.h"

void ALukaHUD::BeginPlay()
{
	Super::BeginPlay();

	ALukaController* LukaController = Cast<ALukaController>(GetOwningPlayerController());
	check(LukaController)
	LukaController->SetInputMode(FInputModeGameOnly());

	check(MainWidgetClass);
	PlayWidget = CreateWidget<UPlayWidget>(LukaController, MainWidgetClass);
	PlayWidget->AddToViewport();

	LukaController->OnPossessToyTransition.AddDynamic(this, &ALukaHUD::OnBeginToyPossessEvent);
	LukaController->OnUnpossessToyTransition.AddDynamic(this, &ALukaHUD::OnEndToyPossessEvent);

	if (ALukaCharacter* Luka = Cast<ALukaCharacter>(LukaController->GetLukaCharacter()))
	{
		Luka->OnBeginAnyInView.AddDynamic(this, &ALukaHUD::OnBeginInteractiveInViewEvent);
		Luka->OnEndAnyInView.AddDynamic(this, &ALukaHUD::OnEndInteractiveInViewEvent);
	}

	if (ensure(PreviousWidgetClass) && ensure(LukaController))
	{
		PreviousWidgetInstance = CreateWidget<UBaseMenuWidget>(LukaController, PreviousWidgetClass);
	}
	if (ensure(SettingsMenuWidgetClass) && ensure(LukaController))
	{
		SettingsMenuWidgetInstance = CreateWidget<USettingsMenuWidget>(LukaController, SettingsMenuWidgetClass);
	}

	if (ensure(EndPanelWidgetClass) && ensure(LukaController))
	{
		EndPanelWidgetInstance = CreateWidget<UCommonActivatableWidget>(LukaController, EndPanelWidgetClass);

		if (ensure(EndPanelWidgetInstance))
		{
			EndPanelWidgetInstance->AddToViewport(10);
			EndPanelWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

UBaseMenuWidget* ALukaHUD::GetPreviousWidget()
{
	check(PreviousWidgetInstance);
	return PreviousWidgetInstance;
}

USettingsMenuWidget* ALukaHUD::GetSettingsMenuWidget() const
{
	check(SettingsMenuWidgetInstance);
	return SettingsMenuWidgetInstance;
}

class UCommonActivatableWidget* ALukaHUD::GetEndPanelWidget() const
{
	check(EndPanelWidgetInstance);
	return EndPanelWidgetInstance;
}

void ALukaHUD::PauseGame()
{
	if (PreviousWidgetInstance)
	{
		PreviousWidgetInstance->AddToViewport(TOP_LEVEL);
		PreviousWidgetInstance->GetFocusedButton()->SetFocus();
	}

	if (ensure(PauseEvent)) 
	{
		FOnAkPostEventCallback NullCallback;
		UAkGameplayStatics::PostEvent(PauseEvent, this, 0, NullCallback);
	}
	
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	GetOwningPlayerController()->SetInputMode(FInputModeUIOnly());
}

void ALukaHUD::ResumeGame()
{
	if (PreviousWidgetInstance)
	{
		// TODO Change to stack
		PreviousWidgetInstance->RemoveFromParent();
	}

	UGameplayStatics::SetGamePaused(GetWorld(), false);
	GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
	
	if (ensure(ResumeEvent)) 
	{
		FOnAkPostEventCallback NullCallback;
		UAkGameplayStatics::PostEvent(ResumeEvent, this, 0, NullCallback);
	}
}

void ALukaHUD::HandleAnswerInput_Implementation() const
{
}

void ALukaHUD::OnBeginInteractiveInViewEvent()
{
	check(PlayWidget);
	check(PlayWidget->GetCursorWidget());
	PlayWidget->GetCursorWidget()->OnEnterPossibleInteraction();
}

void ALukaHUD::OnEndInteractiveInViewEvent()
{
	check(PlayWidget);
	check(PlayWidget->GetCursorWidget());
	PlayWidget->GetCursorWidget()->OnExitPossibleInteraction();
}

void ALukaHUD::OnBeginToyPossessEvent()
{
	check(PlayWidget);
	check(PlayWidget->GetCursorWidget());
	PlayWidget->GetCursorWidget()->SetCursorVisibility(false);
}

void ALukaHUD::OnEndToyPossessEvent()
{
	check(PlayWidget);
	check(PlayWidget->GetCursorWidget());
	PlayWidget->GetCursorWidget()->SetCursorVisibility(true);
}

USubtitlesWidget* ALukaHUD::GetSubtitlesWidget() const
{
	check(PlayWidget);
	return PlayWidget->GetSubtitlesWidget();
}

UAnswerWidget* ALukaHUD::GetAnswerWidget() const
{
	check(PlayWidget);
	return PlayWidget->GetAnswerWidget();
}

UCanvasPanel* ALukaHUD::GetInspectablePanel() const
{
	check(PlayWidget);
	return PlayWidget->GetInspectablePanel();
}

UPromptsHolder* ALukaHUD::GetPromptsHolder() const
{
	check(PlayWidget);
	return PlayWidget->GetPromptsHolder();
}

void ALukaHUD::AddHoverCommandWidget(UHoverCommandWidget* HoverCommandWidget) const
{
	check(PlayWidget);
	PlayWidget->AddHoverCommandWidget(HoverCommandWidget);
}

void ALukaHUD::RemoveHoverCommandWidget(UHoverCommandWidget* HoverCommandWidget) const
{
	check(PlayWidget);
	PlayWidget->RemoveHoverCommandWidget(HoverCommandWidget);
}

void ALukaHUD::AddActiveCommandWidget(UActiveCommandWidget* ActiveCommandWidget) const
{
	check(PlayWidget);
	PlayWidget->AddActiveCommandWidget(ActiveCommandWidget);
}

void ALukaHUD::RemoveActiveCommandWidget(UActiveCommandWidget* ActiveCommandWidget) const
{
	check(PlayWidget);
	PlayWidget->RemoveActiveCommandWidget(ActiveCommandWidget);
}
