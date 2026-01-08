// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Corkboard/CorkboardNoteWidget.h"

#include "CrossCompilerCommon.h"
#include "imgui.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "System/Clues/ClueAsset.h"

void UCorkboardNoteWidget::SetTitleText(FString text)
{
	ClueTitleTextBox->SetText(FText::FromString(text));
}

void UCorkboardNoteWidget::SetShortDescriptionText(FString text)
{
	FString ShortDescText = "<ShortDesc>";
	ShortDescText.Append(text);
	ShortDescText.Append("</>");
	ClueShortDescTextBox->SetText(FText::FromString(ShortDescText));
}

void UCorkboardNoteWidget::SetClueImage(UTexture2D* tex)
{
	ClueImage->SetBrushFromTexture(tex);
}


void UCorkboardNoteWidget::LoadCLue()
{
	check(Clue);

	SetTitleText(Clue->Name);

	if (Clue->DisplayType == EClueDisplayType::Picture)
	{
		ClueShortDescTextBox->SetVisibility(ESlateVisibility::Collapsed);
		SetClueImage(Clue->Image);
	}
	else
	{
		ClueImage->SetVisibility(ESlateVisibility::Collapsed);
		SetShortDescriptionText(Clue->ShortDesc);
	}
}

void UCorkboardNoteWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(ClueTitleTextBox)
	check(ClueShortDescTextBox)
	check(ClueImage)

	PinImage->SetVisibility(ESlateVisibility::Collapsed);

	LoadCLue();
}

FVector2D UCorkboardNoteWidget::GetPinPosition() const
{
	if (!PinImage) { return FVector2D(0, 0); }

	//Get Note Position
	UCanvasPanelSlot* NoteSlot = Cast<UCanvasPanelSlot>(Slot);
	FVector2D Position = NoteSlot->GetPosition();
	FWidgetTransform RTransform = GetRenderTransform();
	FAnchors NoteSlotAnchors = NoteSlot->GetAnchors();
	FVector2D Pivot = GetRenderTransformPivot();
	Position += ((FVector2D(1, 1) - RTransform.Scale) * (NoteSlot->GetSize() * Pivot));
	UCanvasPanelSlot* PinSlot = Cast<UCanvasPanelSlot>(PinImage->Slot);
	Position += PinSlot->GetPosition() * RTransform.Scale;

	return Position;
}

void UCorkboardNoteWidget::SetFakePin(UImage* Pin)
{
	FakePinImage = Pin;
}

UImage* UCorkboardNoteWidget::GetPin() const
{
	return PinImage;
}

UImage* UCorkboardNoteWidget::GetFakePin()
{
	return FakePinImage;
}

void UCorkboardNoteWidget::UpdateStateImage(EClueState state)
{
	StateImage->SetVisibility(ESlateVisibility::Visible);
	switch (state)
	{
	case EClueState::Partial:
		StateImage->SetBrushFromTexture(PartialStateImage);
		break;
	case EClueState::Missing:
		StateImage->SetBrushFromTexture(DisabledStateImage);
		break;
	case EClueState::Complete:
		StateImage->SetVisibility(ESlateVisibility::Collapsed);

		break;
	default:
		break;
	}
}
