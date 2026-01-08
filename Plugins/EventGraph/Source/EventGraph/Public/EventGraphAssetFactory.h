#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "EventGraphAssetFactory.generated.h"

UCLASS()
class UEventGraphAssetFactory : public UFactory
{
	GENERATED_BODY()
public:
	UEventGraphAssetFactory(const FObjectInitializer& ObjectInitializer);

	virtual UObject* FactoryCreateNew(UClass* Class,UObject* Parent, FName Name, EObjectFlags Flags, UObject* Context,FFeedbackContext* Warning);
	virtual bool CanCreateNew() const override;
};