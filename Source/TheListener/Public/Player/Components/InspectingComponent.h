// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InspectingComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THELISTENER_API UInspectingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInspectingComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRead();
	void TryRead(AActor *);
	void TryLeave();

	void MoveSelector(const struct FInputActionInstance&);
	void UseSelector(const struct FInputActionInstance&);

	UPROPERTY(EditAnywhere)
	TArray<FName> ActivePrompts;
	
	// inputs ===========================================
public:
	virtual void SetupInput(UEnhancedInputComponent *);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inspect")
	TArray<FName> Prompts;
	
	UPROPERTY(EditDefaultsOnly, Category = "Inspect|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionInspect;
	UPROPERTY(EditDefaultsOnly, Category = "Inspect|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionReturn;
	UPROPERTY(EditDefaultsOnly, Category = "Inspect|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionSelectorMove;
	UPROPERTY(EditDefaultsOnly, Category = "Inspect|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionSelect;

	UPROPERTY()
	TObjectPtr<class AActor> Inspectable;

	UPROPERTY(EditDefaultsOnly, Category = "Inspect|Enhanced Input")
	TObjectPtr<class UInputMappingContext>  InputMappingContext;
};