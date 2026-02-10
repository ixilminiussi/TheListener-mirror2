#include "UI/LukaHUD.h"

#include "AkGameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "System/Core/BaseGameInstance.h"
#include "UI/CursorWidget.h"
#include "UI/PlayWidget.h"
#include "UI/Menus/PauseMenuWidget.h"
#include "UI/Menus/SettingsMenuWidget.h"
#include "UI/Menus/SettingsSave.h"
#include "UI/Subtitles/SubtitlesWidget.h"

FInteractiveInView ALukaHUD::InteractiveInView;
FOnShowCursor ALukaHUD::OnShowCursor;
FOnHideCursor ALukaHUD::OnHideCursor;

void ALukaHUD::BeginPlay()
{
	Super::BeginPlay();

	check(PlayWidgetClass);
	PlayWidget = CreateWidget<UPlayWidget>(GetWorld()->GetFirstPlayerController(), PlayWidgetClass);
	PlayWidget->AddToViewport();

	OnShowCursor.BindLambda([this]()
	{
		if (this)
		{
			check(PlayWidget);
			check(PlayWidget->GetCursorWidget());
			PlayWidget->GetCursorWidget()->SetVisibility(ESlateVisibility::Visible);
		}
	});
	OnHideCursor.BindLambda([this]()
	{
		if (this)
		{
			check(PlayWidget);
			check(PlayWidget->GetCursorWidget());
			PlayWidget->GetCursorWidget()->SetVisibility(ESlateVisibility::Collapsed);
		}
	});
	InteractiveInView.BindUFunction(this, FName("InViewUpdate"));

	InitializeSettingsData();
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
	PauseGameInBlueprint();

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
	ResumeGameInBlueprint();

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

void ALukaHUD::ToggleCursorVisibility(bool bVisible) const
{
	check(PlayWidget)
	UCursorWidget* Cursor = PlayWidget->GetCursorWidget();
	check(Cursor)

	if (bVisible)
	{
		Cursor->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Cursor->SetVisibility(ESlateVisibility::Collapsed);
	}
}

USubtitlesWidget* ALukaHUD::GetSubtitlesWidget() const
{
	check(PlayWidget)
	return PlayWidget->GetSubtitlesWidget();
}

UAnswerWidget* ALukaHUD::GetAnswerWidget() const
{
	check(PlayWidget)
	return PlayWidget->GetAnswerWidget();
}

UCanvasPanel* ALukaHUD::GetInspectablePanel() const
{
	check(PlayWidget)
	return PlayWidget->GetInspectablePanel();
}

UPromptsHolder* ALukaHUD::GetPromptsHolder() const
{
	check(PlayWidget)
	return PlayWidget->GetPromptsHolder();
}

void ALukaHUD::NotifyClue() const
{
	check(PlayWidget)
	PlayWidget->NotifyClue();
}

void ALukaHUD::InitializeSettingsData()
{
	UBaseGameInstance* GI = Cast<UBaseGameInstance>(GetWorld()->GetGameInstance());
	check(GI);

	USettingsSave* SettingsSave = GI->GetSettingsSave();
	check(SettingsSave);

	USizeBox* CursorSizeBox = PlayWidget->GetCursorWidget()->GetCursorSizeBox();
	check(CursorSizeBox);
	CursorSizeBox->SetHeightOverride(10 * SettingsSave->GetReticleSize());
	CursorSizeBox->SetWidthOverride(10 * SettingsSave->GetReticleSize());

	check(PlayWidget);
	PlayWidget->GetSubtitlesWidget()->SetBakcgroundOpacity(SettingsSave->GetSubtitlesBackgroundOpacityValue());
	PlayWidget->GetSubtitlesWidget()->SetSubtitlesTextStyle(SettingsSave->GetSubtitlesSizeValue());
	PlayWidget->GetSubtitlesWidget()->SetLanguageBoolean(SettingsSave->GetLangageIndex());
}


void ALukaHUD::InViewUpdate(bool bToggle)
{
	check(PlayWidget)
	UCursorWidget* Cursor = PlayWidget->GetCursorWidget();
	check(Cursor)

	if (bInView && !bToggle)
	{
		Cursor->Shrink();
	}
	if (!bInView && bToggle)
	{
		Cursor->Grow();
	}

	bInView = bToggle;
}
