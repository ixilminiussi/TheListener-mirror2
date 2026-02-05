#pragma once
#include "CoreMinimal.h"
#include "BaseMenuWidget.h"

#include "UI/Menus/UIElements/ButtonPrimary.h"
#include "CommonActivatableWidget.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class THELISTENER_API UMainMenuWidget : public UBaseMenuWidget
{
	GENERATED_BODY()

public:
	virtual UButtonPrimary* GetFocusedButton() const override;

	virtual void NativeConstruct() override;

protected:
	//Buttons Functions
	virtual void OnChangeLevelClicked_Implementation() override;

	virtual void OnQuitClicked() const override;

	virtual void OnSettingsClicked() const override;

	void OnGymClicked() const;

	UPROPERTY(meta = (BindWidget))
	UButtonPrimary* GymButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Menus", meta = (AllowedClasses="World"))
	TSoftObjectPtr<UWorld> GymLevel;
};
