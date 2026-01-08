// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Dialogue/AnswerWidget.h"

#include <imgui.h>

#include "CommonButtonBase.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Miscellaneous/TLUtils.h"
#include "System/Dialogue/DialogueSubsystem.h"
#include "UI/Dialogue/AnswerButtonWidget.h"
#include "UI/Dialogue/AnswerDataAsset.h"
#include "UI/Prompt/TimedPromptWidget.h"

void UAnswerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Collapse();

	check(AnswerButtonClass);
}

int32 UAnswerWidget::Ask(FAnswerList const &AnswerList)
{
	static uint32 IDDispenser = 0;
	IDDispenser++;

	check(static_cast<int32>(AnswerList.DefaultAnswer) < AnswerList.Answers.Num()); // Dude, your default answer is too high for the answers you have
	
	uint32 QuestionID = IDDispenser;

	FAnswerOptionsContainer AnswerOptionsContainer{};
	AnswerOptionsContainer.AnswerList = AnswerList;

	constexpr float Interval = 0.01f;

	if (AnswerList.Duration != -1.f)
	{
		GetWorld()->GetTimerManager().SetTimer(AnswerOptionsContainer.ProgressHandle, [this, QuestionID]()
		{
			OnTimerUpdate(QuestionID);
		}, Interval, true);
	
		GetWorld()->GetTimerManager().SetTimer(AnswerOptionsContainer.TimeoutHandle, [this, QuestionID, AnswerList]()
		{
				SelectAnswer(QuestionID, AnswerList.DefaultAnswer); // Clears Timer
		}, AnswerList.Duration, false);
	}

	uint32 i = 0;
	for (const auto& [Answer, NextState] : AnswerList.Answers)
	{
		if (Answer.Equals("hidden"))
		{
			continue;
		}
		UAnswerButtonWidget* ButtonWidget = CreateWidget<UAnswerButtonWidget>(this, AnswerButtonClass);
		if (!ensure(ButtonWidget))
		{
			break;
		}

		ButtonWidget->SetPadding(FMargin{0.0f, Spacing, 0.0f, 0.0f});
		ButtonWidget->SetText(FText::FromString(Answer));
		ButtonWidget->OnClicked().AddLambda([this, QuestionID, i]()
		{
			SelectAnswer(QuestionID, i);
		});

		AnswerOptionsContainer.AnswerButtons.Add(ButtonWidget);
		
		if (ensure(ButtonBox))
		{
			ButtonBox->AddChildToVerticalBox(ButtonWidget);
		}

		i++;
	}

	ChoiceContainers.Add(QuestionID, AnswerOptionsContainer);
	OnAnswerEvents.Add(QuestionID, {});
			
	return QuestionID;
}

void UAnswerWidget::Toggle(const int32 QuestionID)
{
	if (!ensure(ChoiceContainers.Contains(QuestionID)))
	{
		return;
	}
	if (!ensure(ButtonBox))
	{
		return;
	}

	for (UWidget* Widget : ButtonBox->GetAllChildren())
	{
		check(Widget);
		Widget->SetVisibility(ESlateVisibility::Collapsed);
	}

	FAnswerOptionsContainer const &ChoiceContainer = ChoiceContainers[QuestionID];
	for (UAnswerButtonWidget* AnswerButton : ChoiceContainer.AnswerButtons) 
	{
		check(AnswerButton);
		AnswerButton->SetVisibility(ESlateVisibility::Visible);
		AnswerButton->SetIsFocusable(true);
	}

	if (ChoiceContainer.AnswerButtons.Num() > 0)
	{
		SetVisibility(ESlateVisibility::Visible);
		
		APlayerController *PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		check(PlayerController);
		
		ChoiceContainer.AnswerButtons[0]->SetUserFocus(PlayerController);
		
		FInputModeGameAndUI InputMode{};
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(true);
		InputMode.SetWidgetToFocus(ChoiceContainer.AnswerButtons[0]->TakeWidget());
		PlayerController->SetInputMode(InputMode);
	}

	if (ensure(ProgressBar) && ChoiceContainer.AnswerList.Duration > 0)
	{
		ProgressBar->SetVisibility(ESlateVisibility::Visible);
	} else
	{
		ProgressBar->SetVisibility(ESlateVisibility::Collapsed);
	}

	ToggledQuestion = QuestionID;
}

void UAnswerWidget::Collapse()
{
	APlayerController *PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	check(PlayerController);
	PlayerController->SetInputMode(FInputModeGameOnly{});
	SetVisibility(ESlateVisibility::Collapsed);
}

bool UAnswerWidget::IsValid(const int32 QuestionID)
{
	return ChoiceContainers.Contains(QuestionID);
}

void UAnswerWidget::OnTimerUpdate(const int32 QuestionID) const
{
	if (ToggledQuestion == QuestionID)
	{
		if (ensure(ProgressBar))
		{
			if (!ensure(ChoiceContainers.Contains(QuestionID)))
			{
				return;
			}
	
			auto &ChoiceContainer = ChoiceContainers[QuestionID];	
			float Percentage = 1.0f - GetWorld()->GetTimerManager().GetTimerElapsed(ChoiceContainer.TimeoutHandle) / ChoiceContainer.AnswerList.Duration;
			UE_LOG(LogTemp, Warning, TEXT("Progress percentage: %f"), GetWorld()->GetTimerManager().GetTimerElapsed(ChoiceContainer.ProgressHandle));
			
			ProgressBar->SetPercent(Percentage);

			FName RadioAnswerWidgetName = "PRT_RadioAnswer";
			if (UTimedPromptWidget *RadioAnswerWidget = Cast<UTimedPromptWidget>(UTLUtils::GetPrompt(GetWorld(), RadioAnswerWidgetName)))
			{
				RadioAnswerWidget->SetPercentage(Percentage);
			}
		}
	}

}

void UAnswerWidget::SelectAnswer(const int32 QuestionID, const int32 Choice)
{
	if (!ensure(ChoiceContainers.Contains(QuestionID)))
	{
		return;
	}
	
	auto &ChoiceContainer = ChoiceContainers[QuestionID];	
	GetWorld()->GetTimerManager().ClearTimer(ChoiceContainer.ProgressHandle);
	GetWorld()->GetTimerManager().ClearTimer(ChoiceContainer.TimeoutHandle);

	if (!ensure(static_cast<int32>(Choice) < ChoiceContainer.AnswerList.Answers.Num()))
	{
		return;
	}

	FAnswer const &Answer = ChoiceContainer.AnswerList.Answers[Choice];

	if (ensure(OnAnswerEvents.Contains(QuestionID)))
	{
		OnAnswerEvents[QuestionID].Execute(ChoiceContainer.AnswerList.Answers[Choice]);
	}

	for (UAnswerButtonWidget* AnswerButton : ChoiceContainer.AnswerButtons)
	{
		AnswerButton->RemoveFromParent();
	}
	ChoiceContainers.Remove(QuestionID);
	OnAnswerEvents.Remove(QuestionID);

	if (ToggledQuestion == QuestionID)
	{
		Collapse();
	}
}