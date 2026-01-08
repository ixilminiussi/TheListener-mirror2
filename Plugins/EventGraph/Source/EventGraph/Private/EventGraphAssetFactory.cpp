#include "EventGraphAssetFactory.h"

#include "EventGraphAction.h"
#include "EventGraphAsset.h"

UEventGraphAssetFactory::UEventGraphAssetFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SupportedClass = UEventGraphAsset::StaticClass();
}

UObject* UEventGraphAssetFactory::FactoryCreateNew(UClass* Class, UObject* Parent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warning)
{
	UEventGraphAsset* Asset = NewObject<UEventGraphAsset>(Parent,Name,Flags);
	return Asset;
}

bool UEventGraphAssetFactory::CanCreateNew() const
{
	return true;
}
