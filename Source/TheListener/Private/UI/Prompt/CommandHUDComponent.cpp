#include "UI/Prompt/CommandHUDComponent.h"

#include "Player/LukaController.h"
#include "Player/LukaCharacter.h"
#include "UI/LukaHUD.h"
#include "GPE/Toy.h"
#include "GPE/Obji.h"
#include "Kismet/GameplayStatics.h"
#include "InputMappingContext.h"
#include "GPE/File.h"
#include "UI/Prompt/ActiveCommandWidget.h"
#include "UI/Prompt/HoverCommandWidget.h"

UCommandHUDComponent::UCommandHUDComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCommandHUDComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCommandHUDComponent::Generate(class UInputMappingContext* InputMappingContext)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!ensure(PlayerController))
	{
		return;
	}

	AHUD* CurrentHUD = PlayerController->GetHUD();
	check(CurrentHUD);

	if (ALukaHUD* CastedHUD = Cast<ALukaHUD>(CurrentHUD))
	{
		LukaHUD = CastedHUD;
	}

	if (const ALukaController* LukaController = Cast<ALukaController>(PlayerController))
	{
		GlobalMappingContext = LukaController->GetGlobalMappingContext();
		if (const ALukaCharacter *LukaCharacter = LukaController->GetLukaCharacter(); ensure(LukaCharacter))
		{
			LukaMappingContext = LukaCharacter->GetLukaMappingContext();
		}

		ExternalMappingContext = InputMappingContext; // tolerance for nullptr 

		SpawnCommandHelpers(PlayerController);
	}
}

void UCommandHUDComponent::SpawnCommandHelpers(APlayerController* PlayerController)
{
	if (HoverCommandWidgetClass == nullptr)
	{
		return;
	}

	check(GlobalMappingContext)
	check(LukaMappingContext)
	TArray<FEnhancedActionKeyMapping> Mappings = GlobalMappingContext->GetMappings();
	TArray<FEnhancedActionKeyMapping> LukaMappings = LukaMappingContext->GetMappings();

	for (TPair<FString, UInputAction*>& Command : HoverCommands)
	{
		if (Command.Value == nullptr)
		{
			continue;
		}

		bool bHasInputAction = false;
		for (FEnhancedActionKeyMapping ActionKeyMapping : Mappings)
		{
			if (ActionKeyMapping.Action == Command.Value)
			{
				bHasInputAction = true;
				break;
			}
		}
		for (FEnhancedActionKeyMapping ActionKeyMapping : LukaMappings)
		{
			if (ActionKeyMapping.Action == Command.Value)
			{
				bHasInputAction = true;
				break;
			}
		}
		if (!bHasInputAction)
		{
			continue;
		}

		UHoverCommandWidget* NewHoverCommandWidget = CreateWidget<UHoverCommandWidget>(
			PlayerController, HoverCommandWidgetClass);
		check(NewHoverCommandWidget);

		NewHoverCommandWidget->CreateImages();
		NewHoverCommandWidget->SetName(Command.Key);
		NewHoverCommandWidget->SetInputAction(Command.Value);

		HoverCommandWidgets.Add(NewHoverCommandWidget);
	}

	TArray<FEnhancedActionKeyMapping> ActiveMappings{};
	if (ActiveCommandWidgetClass == nullptr)
	{
		return;
	}
	if (ExternalMappingContext != nullptr)
	{
		ActiveMappings = ExternalMappingContext->GetMappings();
	}

	for (TPair<FString, UInputAction*>& Command : ActiveCommands)
	{
		if (Command.Value == nullptr)
		{
			continue;
		}

		bool bHasInputAction = false;
		for (FEnhancedActionKeyMapping ActionKeyMapping : ActiveMappings)
		{
			if (ActionKeyMapping.Action == Command.Value)
			{
				bHasInputAction = true;
				break;
			}
		}
		for (FEnhancedActionKeyMapping ActionKeyMapping : Mappings)
		{
			if (ActionKeyMapping.Action == Command.Value)
			{
				bHasInputAction = true;
				break;
			}
		}
		for (FEnhancedActionKeyMapping ActionKeyMapping : LukaMappings)
		{
			if (ActionKeyMapping.Action == Command.Value)
			{
				bHasInputAction = true;
				break;
			}
		}
		if (!bHasInputAction)
		{
			continue;
		}

		UActiveCommandWidget* NewActiveCommandWidget = CreateWidget<UActiveCommandWidget>(
			PlayerController, ActiveCommandWidgetClass);
		check(NewActiveCommandWidget);

		NewActiveCommandWidget->CreateImages();
		NewActiveCommandWidget->SetName(Command.Key);
		NewActiveCommandWidget->SetInputAction(Command.Value);

		ActiveCommandWidgets.Add(NewActiveCommandWidget);
	}
}

void UCommandHUDComponent::AddHoverToHUD()
{
	if (ensure(LukaHUD))
	{
	for (UHoverCommandWidget* HoverCommandWidget : HoverCommandWidgets)
	{
		check(HoverCommandWidget);
		HoverCommandWidget->ClearImages();
		HoverCommandWidget->UpdateImages(GlobalMappingContext->GetMappings());
		HoverCommandWidget->UpdateImages(LukaMappingContext->GetMappings());
		LukaHUD->AddHoverCommandWidget(HoverCommandWidget);
	}
	}
}

void UCommandHUDComponent::RemoveHoverFromHUD()
{
	for (auto HoverCommandWidget : HoverCommandWidgets)
	{
		check(HoverCommandWidget);
		LukaHUD->RemoveHoverCommandWidget(HoverCommandWidget);
	}
}

void UCommandHUDComponent::AddActiveToHUD()
{
	if (ensure(LukaHUD)) {
	for (auto ActiveCommandWidget : ActiveCommandWidgets)
	{
		check(ActiveCommandWidget);
		ActiveCommandWidget->ClearImages();
		ActiveCommandWidget->UpdateImages(GlobalMappingContext->GetMappings());
		ActiveCommandWidget->UpdateImages(LukaMappingContext->GetMappings());
		if (ExternalMappingContext)
		{
			ActiveCommandWidget->UpdateImages(ExternalMappingContext->GetMappings());
		}
		LukaHUD->AddActiveCommandWidget(ActiveCommandWidget);
	}
	}
}

void UCommandHUDComponent::RemoveActiveFromHUD()
{
	if (ensure(LukaHUD)) {
	for (auto ActiveCommandWidget : ActiveCommandWidgets)
	{
		check(ActiveCommandWidget);
		LukaHUD->RemoveActiveCommandWidget(ActiveCommandWidget);
	}
	}
}
