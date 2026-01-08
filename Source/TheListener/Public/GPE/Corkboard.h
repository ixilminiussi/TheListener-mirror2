// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GPE/Toy.h"
#include "Corkboard.generated.h"

/**
 * 
 */
UCLASS()
class THELISTENER_API ACorkboard : public AToy
{
	GENERATED_BODY()

	ACorkboard();

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|Mesh")
	class UStaticMeshComponent* BoardMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|UI")
	TSubclassOf<class UCorkboardWidget> CorkboardWidgetClass;

	class UCorkboardWidget* CorkboardWidget;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|Enhanced Input")
	class UInputAction* InputActionZoom = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|Enhanced Input")
	class UInputAction* InputActionMove = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|Translation")
	float MoveSpeed = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|Translation")
	float ZoomSpeed = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|Translation")
	FVector2D ZoomLimits;

public:
	void UpdateClue(class UClueAsset* Clue);
	UCorkboardWidget* GetCorkboardWidget();
};
