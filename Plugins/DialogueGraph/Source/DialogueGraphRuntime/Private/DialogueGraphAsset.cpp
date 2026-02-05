// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueGraphAsset.h"
#include "UObject/ObjectSaveContext.h"

#if  UE_EDITOR
void UDialogueGraphAsset::PreSave(FObjectPreSaveContext SaveContext)
{
	UObject::PreSave(SaveContext);
	OnPreSave.Broadcast();
}
#endif
