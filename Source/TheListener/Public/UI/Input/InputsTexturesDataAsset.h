#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputsTexturesDataAsset.generated.h"

UCLASS()
class THELISTENER_API UInputsTexturesDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TMap<struct FKey, int> InputsTextures{};
};