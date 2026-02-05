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
