// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Corkboard/CorkboardWidget.h"

#include "InputActionValue.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "DynamicMesh/MeshTransforms.h"
#include "System/Clues/ClueAsset.h"
#include "UI/Corkboard/CorkboardNoteWidget.h"

bool FNoteLink::operator==(TTuple<UCorkboardNoteWidget*, UCorkboardNoteWidget*> CWs) const
{
	return (CWs.Key == CLueWidgetA && CWs.Value == CLueWidgetB) || (CWs.Key == CLueWidgetB && CWs.Value == CLueWidgetA);
}

void FNoteLink::Init(UCorkboardNoteWidget* CWA, UCorkboardNoteWidget* CWB, TObjectPtr<UWidgetTree> WidgetTree,
                     UCanvasPanel* ContentPanel, float LineThickness, UTexture2D* LineTexture)
{
	CLueWidgetA = CWA;
	CLueWidgetB = CWB;

	if (WidgetTree && ContentPanel)
	{
		FString Name = "Link";
		Name.Append(CWA->Clue->Name);
		Name.Append(CWB->Clue->Name);
		Image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), FName(Name));
		ContentPanel->AddChild(Image);

		FVector2D PinAPos = CLueWidgetA->GetPinPosition() - FVector2D(0, LineThickness / 2);
		FVector2D PinBPos = CLueWidgetB->GetPinPosition() - FVector2D(0, LineThickness / 2);

		UCanvasPanelSlot* LinkPanelSlot = Cast<UCanvasPanelSlot>(Image->Slot);
		FAnchors Anchor(0.5f, 0.5f);
		LinkPanelSlot->SetAnchors(Anchor);
		Image->SetRenderTransformPivot(FVector2D(0.0f, 0.5f));


		LinkPanelSlot->SetPosition(PinAPos);
		LinkPanelSlot->SetZOrder(5);
		float Distance = FVector2D::Distance(PinAPos, PinBPos);
		Image->SetBrushFromTexture(LineTexture);
		FSlateBrush Brush = Image->GetBrush();
		Brush.DrawAs = ESlateBrushDrawType::Type::Image;
		Brush.Tiling = ESlateBrushTileType::Type::Horizontal;
		Image->SetBrush(Brush);
		FVector2D DirVector = FVector2D(PinBPos - PinAPos);
		float Angle = FMath::Atan2(DirVector.Y, DirVector.X);
		FWidgetTransform WTransform = Image->GetRenderTransform();
		WTransform.Angle = FMath::RadiansToDegrees(Angle);
		LinkPanelSlot->SetSize(FVector2D(Distance, LineThickness));
		Image->SetRenderTransform(WTransform);
	}
}

void UCorkboardWidget::SetMoveSpeed(float InSpeed)
{
	MoveSpeed = InSpeed;
}

void UCorkboardWidget::SetZoomSpeed(float InZoomSpeed)
{
	ZoomSpeed = InZoomSpeed;
}

void UCorkboardWidget::SetZoomLimits(FVector2D InZoomLimits)
{
	ZoomLimits = InZoomLimits;
}

void UCorkboardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	check(ContentCanvasPanel);
	ContentCanvasPanelSlot = Cast<UCanvasPanelSlot>(ContentCanvasPanel->Slot);
	check(ContentCanvasPanelSlot);
	BaseSize = ContentCanvasPanelSlot->GetSize();
	CurrentAlignment = ContentCanvasPanelSlot->GetAlignment();
}

void UCorkboardWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!MoveDirection.IsZero())
	{
		Move(InDeltaTime);
	}
	Zoom(InDeltaTime);
}


void UCorkboardWidget::MoveInput(const FInputActionValue& Value)
{
	MoveDirection = Value.Get<FVector2D>();
	MoveDirection.Y *= -1;
}

void UCorkboardWidget::ZoomInput(const struct FInputActionValue& Value)
{
	ZoomLastInput = Value.Get<float>();
}

void UCorkboardWidget::AddLink(UCorkboardNoteWidget* LinkA, UCorkboardNoteWidget* LinkB)
{
	FNoteLink NewLink;
	NewLink.Init(LinkA, LinkB, WidgetTree, ContentCanvasPanel, LinkThickness, LinkTexture);
	NoteLinks.Add(NewLink);
}

void UCorkboardWidget::Reset()
{
	ContentCanvasPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	CurrentZoom = 1;
	CurrentAlignment = FVector2D(0.5f);
	ContentCanvasPanelSlot->SetSize(BaseSize * CurrentZoom);
}

void UCorkboardWidget::UpdateClue(class UClueAsset* Clue)
{
	if (!NoteWidgets.Contains(Clue)) { return; }
	UCorkboardNoteWidget* ActualClueWidget = NoteWidgets[Clue];
	switch (Clue->State)
	{
	case EClueState::Hidden:
	case EClueState::Disabled:
		{
			ActualClueWidget->SetVisibility(ESlateVisibility::Collapsed);
			ActualClueWidget->GetFakePin()->SetVisibility(ESlateVisibility::Collapsed);
			break;
		}

	case EClueState::Partial:
	case EClueState::Complete:
		{
			ActualClueWidget->SetVisibility(ESlateVisibility::Visible);
			ActualClueWidget->GetFakePin()->SetVisibility(ESlateVisibility::Visible);
			break;
		}
	default:
		break;
	}
	ActualClueWidget->UpdateStateImage(Clue->State);
	//Update Surrounding
	for (UClueAsset* Link : Clue->Links)
	{
		if (!NoteWidgets.Contains(Link)) { continue; }
		NoteWidgets[Link]->UpdateStateImage(Link->State);
		if (!(Clue->IsFound() && Link->IsFound())) { continue; }
		if (!NoteLinks.Contains(
			TTuple<UCorkboardNoteWidget*, UCorkboardNoteWidget*>(NoteWidgets[Clue], NoteWidgets[Link])))
		{
			AddLink(NoteWidgets[Clue], NoteWidgets[Link]);
		}
	}
}

void UCorkboardWidget::Move(float delta)
{
	CurrentAlignment = FVector2D::Clamp(CurrentAlignment + (MoveDirection * delta * (MoveSpeed * (1 / CurrentZoom))),
	                                    FVector2D::ZeroVector, FVector2D(1, 1));
	ContentCanvasPanelSlot->SetAlignment(CurrentAlignment);
}

void UCorkboardWidget::Zoom(float delta)
{
	CurrentZoom = FMath::Clamp(CurrentZoom + ZoomLastInput * delta * ZoomSpeed, ZoomLimits.X, ZoomLimits.Y);
	FWidgetTransform ContentTransform = ContentCanvasPanel->GetRenderTransform();
	ContentTransform.Scale = FVector2D(CurrentZoom, CurrentZoom);
	ContentCanvasPanel->SetRenderTransformPivot(ContentCanvasPanelSlot->GetAlignment());
	ContentCanvasPanel->SetRenderTransform(ContentTransform);
}

void UCorkboardWidget::SetupNoteWidgets()
{
	NoteWidgets.Empty();
	for (UWidget* Widget : ContentCanvasPanel->GetAllChildren())
	{
		if (UCorkboardNoteWidget* Note = Cast<UCorkboardNoteWidget>(Widget))
		{
			if (NoteWidgets.Contains(Note->Clue))
			{
				UE_LOG(LogTemp, Warning, TEXT("Duplicate Clue in Corkboard"));
				continue;
			}
			NoteWidgets.Add(Note->Clue, Note);

			if (WidgetTree)
			{
				FString Name = "FakePinImage";
				Name.AppendInt(NoteWidgets.Num());
				UImage* FakePinImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), FName(Name));
				UImage* ActualPin = Note->GetPin();
				ContentCanvasPanel->AddChild(FakePinImage);

				UCanvasPanelSlot* FakePinSlot = Cast<UCanvasPanelSlot>(FakePinImage->Slot);
				UCanvasPanelSlot* ActualPinSlot = Cast<UCanvasPanelSlot>(ActualPin->Slot);

				FakePinSlot->SetZOrder(100);

				FakePinSlot->SetAnchors(FAnchors(0.5f, 0.5f));
				//FVector2D Size = 
				FakePinSlot->SetSize(ActualPinSlot->GetSize());
				FakePinSlot->SetAlignment(ActualPinSlot->GetAlignment());

				FakePinImage->SetBrush(ActualPin->GetBrush());

				FakePinSlot->SetPosition(Note->GetPinPosition());


				FakePinImage->SetVisibility(ESlateVisibility::Collapsed);
				Note->SetFakePin(FakePinImage);
			}
			Note->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
