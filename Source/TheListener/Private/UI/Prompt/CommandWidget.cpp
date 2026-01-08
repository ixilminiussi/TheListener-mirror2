#include "UI/Prompt/CommandWidget.h"

#include "EnhancedActionKeyMapping.h"
#include "Blueprint/WidgetTree.h"
#include "CommonUI/Public/CommonTextBlock.h"
#include "CommonUI/Public/CommonLazyImage.h"
#include "Components/HorizontalBox.h"
#include "Kismet/GameplayStatics.h"
#include "System/Core/BaseGameInstance.h"
#include "Styling/SlateBrush.h"


void UCommandWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (auto GameInstance = Cast<UBaseGameInstance>(GetGameInstance()))
	{
		GameInstance->OnChangeControllerType.AddDynamic(this, &UCommandWidget::ModifyControllerTextures);
	}
}

void UCommandWidget::CreateImages() const
{
	for (size_t i = 0; i < ImagesLimit; i++)
	{
		UCommonLazyImage* Image = WidgetTree->ConstructWidget<UCommonLazyImage>(UCommonLazyImage::StaticClass());
		Image->SetVisibility(ESlateVisibility::Collapsed);
		ImagesHorizontalBox->AddChildToHorizontalBox(Image);
	}
}

void UCommandWidget::SetName(const FString& Name)
{
	this->NameText = Name;

	check(InputText);
	InputText->SetText(FText::FromString(Name));
}

void UCommandWidget::SetInputAction(UInputAction* InputAction)
{
	AssociatedInputAction = InputAction;
}

void UCommandWidget::ClearImages()
{
	check(ImagesHorizontalBox);
	for (auto CurrentSlot : ImagesHorizontalBox->GetSlots())
	{
		UCommonLazyImage* Image = Cast<UCommonLazyImage>(CurrentSlot->Content);
		Image->SetVisibility(ESlateVisibility::Collapsed);
	}
	IDs.Empty();
}

void UCommandWidget::UpdateImages(const TArray<FEnhancedActionKeyMapping> Mapping)
{
	if (AssociatedInputAction == nullptr) return;
	check(ImagesHorizontalBox);

	if (UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		int Index = 0;
		
		for (auto HoverKeyMapping : Mapping)
		{
			if (AssociatedInputAction == HoverKeyMapping.Action)
			{
				if (!HoverKeyMapping.Key.IsGamepadKey()) continue;

				int MappingId = GameInstance->FindAssociatedInputMappingId(HoverKeyMapping.Key);
				IDs.Add(MappingId);
				UCommonLazyImage* Image = Cast<UCommonLazyImage>(ImagesHorizontalBox->GetChildAt(Index));
				check(Image);

				Index++;

				FSlateBrush Brush;
				Brush.SetResourceObject(GameInstance->GetCurrentControllerTypeTexture(MappingId));
				Brush.ImageSize = FVector2D(48.f, 48.f);
				
				Image->SetBrush(Brush);
				Image->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UCommandWidget::ModifyControllerTextures()
{
	if (IDs.IsEmpty()) return;
	
	if (UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		for (int i = 0; i < IDs.Num() ; i++)
		{
			UCommonLazyImage* Image = Cast<UCommonLazyImage>(ImagesHorizontalBox->GetChildAt(i));
			if (!Image) return;

			if (auto Texture = GameInstance->GetCurrentControllerTypeTexture(IDs[i]))
			{
				FSlateBrush Brush;
				Brush.SetResourceObject(Texture);
				Brush.ImageSize = FVector2D(48.f, 48.f);
						
				Image->SetBrush(Brush);
				Image->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}