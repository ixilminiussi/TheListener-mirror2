#include "UI/InspectWidget.h"

#include "EventSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "System/Events/EventCondition.h"
#include "UI/LukaHUD.h"
#include "UI/SelectorBox.h"


USelectorBox* UInspectWidget::CheckSelectorBoxes(FVector2D SelectorPosition)
{
	bool SelectorBoxHovered = false;
	USelectorBox* HoveredSelectorBox = nullptr;
	for (USelectorBox* Widget : SelectorBoxes)
	{
		if (Widget->IsSelectorInside(SelectorPosition))
		{
			SelectorBoxHovered = true;
			HoveredSelectorBox = Widget;
		}
	}
	if (SelectorBoxHovered)
	{
		FString BoxName = HoveredSelectorBox->GetSelectorName();
		if (!BoxName.IsEmpty())
		{
			for (USelectorBox* SelectedBox : SelectorBoxesMap[BoxName])
			{
				SelectedBox->SelectorHover();
			}
		}
	}
	return HoveredSelectorBox;
}

void UInspectWidget::MoveSelector(FVector2D Value) 
{
	if (!bHasSelector && !ensure(SelectorImage)) {return;}
	UCanvasPanelSlot* CPSlot = Cast<UCanvasPanelSlot>(SelectorImage->Slot);
	Value.Y = Value.Y * -1;
	CPSlot->SetPosition(CPSlot->GetPosition() + Value*MoveSpeed);
	FVector2D NewPos = CPSlot->GetPosition();
	USelectorBox* HoveredBox = CheckSelectorBoxes(NewPos);
	FVector2D SSize = FVector2D(HoveredBox != nullptr ? CurrentSelectorSize * SelectorSizeMultiplier : CurrentSelectorSize);
	CPSlot->SetSize(SSize);
}

void UInspectWidget::Select()
{
	if (!bHasSelector && !ensure(SelectorImage)) {return;}
	FVector2D SelectorPos = Cast<UCanvasPanelSlot>(SelectorImage->Slot)->GetPosition();
	USelectorBox* HoveredSelectorBox = CheckSelectorBoxes(SelectorPos);
	if (HoveredSelectorBox != nullptr)
	{
		FString GroupName = HoveredSelectorBox->GetSelectorName();
		if (!GroupName.IsEmpty())
		{
			for (USelectorBox* SelectedBox : SelectorBoxesMap[GroupName])
			{
				SelectedBox->Discover();
			}
			FConditionKey CKey = USelectorHighlightCondition::GenerateKey(GetClass(),GroupName);
			GetWorld()->GetSubsystem<UEventSubsystem>()->SetConditionValue(CKey,true,false);
			OnSelectorUse.Broadcast(GroupName);
		}
	}
	
}

void UInspectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (bHasSelector && ensure(SelectorImage))
	{
		SelectorBoxes.Empty();
		SelectorBoxesMap.Empty();
		TArray<UWidget*> Widgets;
		WidgetTree->GetAllWidgets(Widgets);
		CurrentSelectorSize = Cast<UCanvasPanelSlot>(SelectorImage->Slot)->GetSize().X;
		for (UWidget* Widget : Widgets)
		{
			if (USelectorBox* CastWidget = Cast<USelectorBox>(Widget))
			{
				SelectorBoxes.Add(CastWidget);
				UCanvasPanelSlot* WSlot = Cast<UCanvasPanelSlot>(CastWidget->Slot);
				CastWidget->SetSlotInformation(WSlot->GetPosition(),WSlot->GetSize());
				FString Boxname = CastWidget->GetSelectorName();
				//Yo you forgot To put a name to this selector this will not behave like wanted ( will not highlight)
				if (!ensureAlways(Boxname != "")) {continue;}
				if (!SelectorBoxesMap.Contains(Boxname)){SelectorBoxesMap.Add(Boxname);}
				SelectorBoxesMap[Boxname].Add(CastWidget);
				
			}
		}
	}
}
