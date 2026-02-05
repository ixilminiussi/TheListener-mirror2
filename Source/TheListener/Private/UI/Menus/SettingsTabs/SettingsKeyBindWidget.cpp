// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "UI/Menus/SettingsTabs/SettingsKeyBindWidget.h"

#include "CommonTextBlock.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Components/InputKeySelector.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "TheListener/TheListener.h"
#include "UI/Menus/UIElements/KeyBindButton.h"
#include "UI/Prompt/PromptImage.h"
#include "UserSettings/EnhancedInputUserSettings.h"


void USettingsKeyBindWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsKeyBind = true;
	DisplayKeys();
}

void USettingsKeyBindWidget::NativeOnActivated()
{
	ScrollBox->GetChildAt(1)->SetFocus();
}

void USettingsKeyBindWidget::DisplayKeys()
{
	if (!KeyBindsButtons.IsEmpty())
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		return;
	}

	UEnhancedInputUserSettings* UserSettings = Subsystem->GetUserSettings();
	if (!UserSettings)
	{
		return;
	}

	TSet<FString> AddedKeys;
	TMap<FName, int32> ActionInstanceCount;
	TMap<FName, int32> ActionTotalCounts;

	for (UInputMappingContext* Map : AllMaps)
	{
		if (!Map)
		{
			continue;
		}

		TArray<FEnhancedActionKeyMapping> Bindings = Map->GetMappings();

		for (const FEnhancedActionKeyMapping& KeyMapping : Bindings)
		{
			if (!KeyMapping.IsPlayerMappable())
			{
				continue;
			}

			ActionTotalCounts.FindOrAdd(KeyMapping.Action->GetFName())++;
		}
	}

	for (UInputMappingContext* Map : AllMaps)
	{
		if (!Map)
		{
			continue;
		}

		TArray<FEnhancedActionKeyMapping> Bindings = Map->GetMappings();

		bool bHasValidBindings = false;
		for (const FEnhancedActionKeyMapping& KeyMapping : Bindings)
		{
			if (!KeyMapping.IsPlayerMappable())
			{
				continue;
			}

			bHasValidBindings = true;
			break;
		}
		if (!bHasValidBindings)
		{
			continue;
		}

		check(ScrollBox);
		UCommonTextBlock* ContextHeader = NewObject<UCommonTextBlock>(this);
		if (ContextHeader)
		{
			ContextHeader->SetText(FText::FromString(Map->GetName()));
			ContextHeader->SetStyle(TextStyle);
			ContextHeader->SetIsEnabled(false);
			ScrollBox->AddChild(ContextHeader);
		}

		TArray<UKeyBindButton*> ButtonsThisMap;

		for (FEnhancedActionKeyMapping KeyMapping : Bindings)
		{
			if (!KeyMapping.IsPlayerMappable())
			{
				continue;
			}

			if (const FPlayerKeyMapping* PlayerMap =
				UserSettings->FindCurrentMappingForSlot(
					KeyMapping.GetMappingName(),
					EPlayerMappableKeySlot::First
				))
			{
				KeyMapping.Key = PlayerMap->GetCurrentKey();
			}

			int32 Total = ActionTotalCounts.FindChecked(KeyMapping.Action->GetFName());
			int32& Count = ActionInstanceCount.FindOrAdd(KeyMapping.Action->GetFName());
			Count++;

			FString UniqueKey = FString::Printf(
				TEXT("%s|%s|%s|%d"),
				*KeyMapping.Action->GetName(),
				*KeyMapping.Key.GetFName().ToString(),
				*Map->GetName(),
				Count
			);

			if (AddedKeys.Contains(UniqueKey))
			{
				continue;
			}

			AddedKeys.Add(UniqueKey);

			check(KeyBindButtonClass);
			UKeyBindButton* KeyBindButton = CreateWidget<UKeyBindButton>(GetWorld(), KeyBindButtonClass);
			check(KeyBindButton);

			KeyBindButton->OnRebindCaptured.AddDynamic(this, &USettingsKeyBindWidget::HandleKeyRebound);

			KeyBindsButtons.Add(KeyBindButton);
			ButtonsThisMap.Add(KeyBindButton);

			FString DisplayName = KeyMapping.GetDisplayName().ToString();
			if (Total > 1)
			{
				if (Count == 1)
				{
					DisplayName += TEXT(" Left");
				}
				else if (Count == 2)
				{
					DisplayName += TEXT(" Right");
				}
				else
				{
					DisplayName += FString::Printf(TEXT(" (%d)"), Count);
				}
			}

			KeyBindButton->Init(
				KeyMapping.GetMappingName(),
				FText::FromString(DisplayName),
				KeyMapping
			);

			ButtonToContext.Add(KeyBindButton, Map);
			ButtonToMapping.Add(KeyBindButton, KeyMapping);

			ScrollBox->AddChild(KeyBindButton);
		}

		{
			TMap<FKey, int32> KeyCountForMap;
			for (UKeyBindButton* Btn : ButtonsThisMap)
			{
				FKey Key = Btn->GetBoundKey();
				KeyCountForMap.FindOrAdd(Key)++;
			}

			for (UKeyBindButton* Btn : ButtonsThisMap)
			{
				FKey Key = Btn->GetBoundKey();

				if (KeyCountForMap.Contains(Key) && KeyCountForMap[Key] > 1)
				{
					Btn->GetPromptImage()->SetColorAndOpacity(FLinearColor::Red);
				}
				else
				{
					Btn->GetPromptImage()->SetColorAndOpacity(FLinearColor::White);
				}
			}
		}
	}

	SetupNavigation();
}

void USettingsKeyBindWidget::SetupNavigation()
{
	TArray<UKeyBindButton*> VisualButtons;

	for (int32 i = 0; i < ScrollBox->GetChildrenCount(); i++)
	{
		UWidget* Child = ScrollBox->GetChildAt(i);
		if (UKeyBindButton* Button = Cast<UKeyBindButton>(Child))
		{
			VisualButtons.Add(Button);
		}
	}

	const int32 NumButtons = VisualButtons.Num();
	if (NumButtons < 2)
	{
		return;
	}

	for (int32 i = 0; i < NumButtons; i++)
	{
		UKeyBindButton* CurrentButton = VisualButtons[i];
		if (!CurrentButton)
		{
			continue;
		}

		const int32 PreviousIndex = (i - 1 + NumButtons) % NumButtons;
		const int32 NextIndex = (i + 1) % NumButtons;

		CurrentButton->SetNavigationRuleExplicit(EUINavigation::Up, VisualButtons[PreviousIndex]);
		CurrentButton->SetNavigationRuleExplicit(EUINavigation::Down, VisualButtons[NextIndex]);
	}
}

void USettingsKeyBindWidget::HandleKeyRebound(UKeyBindButton* Button, FKey NewKey)
{
	if (!Button || !ButtonToContext.Contains(Button))
	{
		return;
	}

	UInputMappingContext* TargetContext = ButtonToContext[Button];
	const FEnhancedActionKeyMapping& OldMapping = ButtonToMapping[Button];

	APlayerController* PC = GetOwningPlayer();
	check(PC);

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	UEnhancedInputUserSettings* UserSettings = Subsystem->GetUserSettings();
	if (!UserSettings)
	{
		LOG_NAKED("UserSettings is Missing !");
	}

	if (!UserSettings->IsMappingContextRegistered(TargetContext))
	{
		UserSettings->RegisterInputMappingContext(TargetContext);
	}

	FGameplayTagContainer FailureReason;
	FMapPlayerKeyArgs MapArgs;
	MapArgs.MappingName = OldMapping.GetMappingName();
	MapArgs.NewKey = NewKey;
	MapArgs.Slot = EPlayerMappableKeySlot::First;

	UserSettings->UnMapPlayerKey(MapArgs, FailureReason);
	UserSettings->MapPlayerKey(MapArgs, FailureReason);

	UserSettings->SaveSettings();
	UserSettings->ApplySettings();

	Button->UpdateVisual(NewKey);

	TMap<FKey, int32> KeyCount;

	for (UKeyBindButton* Btn : KeyBindsButtons)
	{
		if (!Btn || !ButtonToContext.Contains(Btn))
		{
			continue;
		}

		if (ButtonToContext[Btn] != TargetContext)
		{
			continue;
		}

		FKey Key = Btn->GetBoundKey();
		KeyCount.FindOrAdd(Key)++;
	}

	for (UKeyBindButton* Btn : KeyBindsButtons)
	{
		if (!Btn || !ButtonToContext.Contains(Btn))
		{
			continue;
		}

		if (ButtonToContext[Btn] != TargetContext)
		{
			continue;
		}

		FKey Key = Btn->GetBoundKey();

		if (KeyCount.Contains(Key) && KeyCount[Key] > 1)
		{
			Btn->CurrentColor = FLinearColor::Red;
			Btn->GetPromptImage()->SetColorAndOpacity(Btn->CurrentColor);
		}
		else
		{
			Btn->CurrentColor = FLinearColor::Red;
			Btn->GetPromptImage()->SetColorAndOpacity(Btn->CurrentColor);
		}
	}
}
