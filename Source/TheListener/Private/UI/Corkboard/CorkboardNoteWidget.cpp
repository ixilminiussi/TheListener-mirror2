// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Corkboard/CorkboardNoteWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "System/Clues/ClueAsset.h"

void UCorkboardNoteWidget::SetClueImage(UTexture2D* tex)
{
	ClueImage->SetBrushFromTexture(tex);
}


void UCorkboardNoteWidget::LoadCLue()
{
	check(Clue);

	SetClueImage(ClueTexture);
}

void UCorkboardNoteWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(ClueImage)

	if (!bHasPin)
	{
		PinImage->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		PinImage->SetVisibility(ESlateVisibility::Visible);
	}

	LoadCLue();
}

FVector2D UCorkboardNoteWidget::GetPinPosition() const
{
	if (!PinImage) { return FVector2D(0, 0); }

	//Get Note Position
	UCanvasPanelSlot* NoteSlot = Cast<UCanvasPanelSlot>(Slot);
	//FVector2D Offset = FVector2D(400,300);
	//Offset += FVector2D(0,GetRenderTransform().Scale)
	FVector2D Position = NoteSlot->GetPosition();
	//LOG("Pin Position : %s", *Position.ToString())
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
	Cast<UCanvasPanelSlot>(FakePinImage->Slot)->SetSize(FVector2D(50, 50));
}

UImage* UCorkboardNoteWidget::GetPin() const
{
	return PinImage;
}

UImage* UCorkboardNoteWidget::GetFakePin() const
{
	return FakePinImage;
}

void UCorkboardNoteWidget::UpdateStateImage(EClueState state)
{
	/*
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
	}*/
}
