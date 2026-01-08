// Fill out your copyright notice in the Description page of Project Settings.


#include "EventGraphAsset.h"
#include "UObject/ObjectSaveContext.h"

#if UE_EDITOR
#include "EventGraph/Public/EventGraphEditorApp.h"

void UEventGraphAsset::PreSave(FObjectPreSaveContext SaveContext)
{
	UObject::PreSave(SaveContext);
	OnPreSave.Broadcast();
}
#endif
