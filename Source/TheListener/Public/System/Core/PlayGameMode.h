#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PlayGameMode.generated.h"

UCLASS()
class THELISTENER_API APlayGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;	
};