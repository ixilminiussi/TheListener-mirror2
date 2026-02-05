// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InspectableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UInspectableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInspectableComponent();

	void ShowWidget();
	void HideWidget();
	void MoveSelector(FVector2D const &);
	void Select();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Inspectable") // TODO: Change down to line to DefaultOnly
	TSubclassOf<class UInspectWidget> InspectClass;

	UPROPERTY()
	TObjectPtr<class UInspectWidget> InspectWidget;
};
