// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueGraphAssetFactory.h"

#include "DialogueGraphAsset.h"

UDialogueGraphAssetFactory::UDialogueGraphAssetFactory(const FObjectInitializer& ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UDialogueGraphAsset::StaticClass();
}

UObject* UDialogueGraphAssetFactory::FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warning)
{
	return NewObject<UDialogueGraphAsset>(Parent,Name,Flags);
}

bool UDialogueGraphAssetFactory::CanCreateNew() const
{
	return true;
}
