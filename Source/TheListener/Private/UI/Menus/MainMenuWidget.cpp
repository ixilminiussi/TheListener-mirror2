#include "UI/Menus/MainMenuWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Overlay.h"
#include "Kismet/GameplayStatics.h"

UButtonPrimary* UMainMenuWidget::GetFocusedButton() const
{
	return ChangeLevelButton;
}

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(this->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	check(GymButton);
	GymButton->OnClicked().AddUObject(this, &UMainMenuWidget::OnGymClicked);
}

void UMainMenuWidget::OnChangeLevelClicked_Implementation()
{
	Super::OnChangeLevelClicked_Implementation();
}

void UMainMenuWidget::OnQuitClicked() const
{
	Super::OnQuitClicked();
}

void UMainMenuWidget::OnSettingsClicked() const
{
	Super::OnSettingsClicked();
}

void UMainMenuWidget::OnGymClicked() const
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), GymLevel);
}

void UMainMenuWidget::SetFocusOnMainMenu()
{
	PreMenuOverlay->SetVisibility(ESlateVisibility::Collapsed);

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	PlayerController->FlushPressedKeys();

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(
		Handle,
		[this]()
		{
			if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController(); PlayerController)
			{
				PlayerController->SetInputMode(FInputModeUIOnly().SetWidgetToFocus(GetFocusedButton()->TakeWidget()));
			}
		},
		0.01f,
		false
	);
}
