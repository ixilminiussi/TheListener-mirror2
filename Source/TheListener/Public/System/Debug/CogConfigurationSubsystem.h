#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CogConfigurationSubsystem.generated.h"

UCLASS()
class THELISTENER_API UCogConfigurationSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void PostInitialize() override;

protected:
	UPROPERTY()
	TWeakObjectPtr<USubsystem> CogSubsystem;
};
