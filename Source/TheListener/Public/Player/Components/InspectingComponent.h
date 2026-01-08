// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InspectingComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THELISTENER_API UInspectingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInspectingComponent();
	
	class UInputMappingContext* GetInputMappingContext() const;

protected:
	virtual void BeginPlay() override;

	void Leave(const struct FInputActionInstance&);
	void Read(const struct FInputActionInstance&);
	void ReadFile(TScriptInterface<class IInspectable> InspectableToRead);

	void MoveSelector(const struct FInputActionInstance&);
	void UseSelector(const struct FInputActionInstance&);

	class ALukaCharacter* LukaCharacter;

	TWeakObjectPtr<class UEnhancedInputLocalPlayerSubsystem> WeakInputLocalPlayerSubsystem;
	
	UPROPERTY()
	TScriptInterface<class IInspectable> Inspectable;

	UPROPERTY(EditAnywhere)
	class UCommandHUDComponent* CommandHUDComponent = nullptr;

	// inputs ===========================================
public:
	virtual void SetupInput(UEnhancedInputComponent*,
	                        UEnhancedInputLocalPlayerSubsystem*);

protected:
	UPROPERTY(EditAnywhere, Category = "ReadingComponent|Enhanced Input")
	class UInputAction* InputActionRead;
	UPROPERTY(EditAnywhere, Category = "ReadingComponent|Enhanced Input")
	class UInputAction* InputActionLeave;
	UPROPERTY(EditAnywhere, Category = "ReadingComponent|Enhanced Input")
	class UInputAction* InputActionSelectorMove;
	UPROPERTY(EditAnywhere, Category = "ReadingComponent|Enhanced Input")
	class UInputAction* InputActionSelect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ReadingComponent|Enhanced Input")
	class UInputMappingContext* InputMappingContext;
	// inputs ===========================================
};
