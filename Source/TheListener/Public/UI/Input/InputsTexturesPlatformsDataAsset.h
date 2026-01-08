#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputsTexturesPlatformsDataAsset.generated.h"

USTRUCT()
struct FTexturePair
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	int ID = 0;
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* Generic = nullptr;
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* Playstation = nullptr;
};

UCLASS()
class THELISTENER_API UInputsTexturesPlatformsDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TArray<FTexturePair> AssociatedTextures{};
};
