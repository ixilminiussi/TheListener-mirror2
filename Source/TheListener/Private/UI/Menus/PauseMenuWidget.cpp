// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menus/PauseMenuWidget.h"

#include "Kismet/GameplayStatics.h"
#include "UI/BaseHUD.h"
#include "UI/LukaHUD.h"
#include "UI/Menus/SettingsMenuWidget.h"

UButtonPrimary* UPauseMenuWidget::GetFocusedButton() const
{
	return ResumeButton;
}

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(ResumeButton);
	ResumeButton->OnClicked().AddUObject(this, &UPauseMenuWidget::OnResumeClicked);
}

FReply UPauseMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Right)
	{
		OnResumeClicked();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UPauseMenuWidget::OnResumeClicked() const
{
	const APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ALukaHUD* HUD = Cast<ALukaHUD>(PC->GetHUD());

	if (ensure(HUD))
	{
		HUD->ResumeGame();
	}
}

void UPauseMenuWidget::OnSettingsClicked() const
{
	Super::OnSettingsClicked();
}

void UPauseMenuWidget::OnChangeLevelClicked_Implementation()
{
	Super::OnChangeLevelClicked();
}

void UPauseMenuWidget::OnQuitClicked() const
{
	Super::OnQuitClicked();
}
