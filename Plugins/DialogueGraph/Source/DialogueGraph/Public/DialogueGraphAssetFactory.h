// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "DialogueGraphAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class DIALOGUEGRAPH_API UDialogueGraphAssetFactory : public UFactory
{
	GENERATED_BODY()
public:
	UDialogueGraphAssetFactory(const FObjectInitializer& ObjectInitializer);

	virtual UObject* FactoryCreateNew(UClass* Class,UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context,FFeedbackContext* Warning) override;
	virtual bool CanCreateNew() const override;
};
