// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.


#include "UI/Prompt/PromptImage.h"

#include "EnhancedActionKeyMapping.h"
#include "Kismet/GameplayStatics.h"
#include "System/Core/BaseGameInstance.h"
#include "EnhancedInput/Public/EnhancedActionKeyMapping.h"

void UPromptImage::Refresh(TArray<FEnhancedActionKeyMapping> const &Mapping)
{
	if (UBaseGameInstance* GameInstance = Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		int i = 0;
		for (auto HoverKeyMapping : Mapping)
		{
			if (InputAction == HoverKeyMapping.Action)
			{
				if (i == Index)
				{
					
					if (!HoverKeyMapping.Key.IsGamepadKey()) continue;

					const int ID = GameInstance->FindAssociatedInputMappingId(HoverKeyMapping.Key);

					FSlateBrush NewBrush;
					NewBrush.SetResourceObject(GameInstance->GetCurrentControllerTypeTexture(ID));
					NewBrush.ImageSize = FVector2D(48.f, 48.f);
				
					SetBrush(NewBrush);
					return;
				}
				i ++;
			}
		}
	}
}
