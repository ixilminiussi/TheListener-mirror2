#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "CogPlottingSubsystem.generated.h"

UCLASS()
class THELISTENER_API UCogPlottingSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { return GetStatID(); }

protected:
	UPROPERTY()
	TWeakObjectPtr<USubsystem> CogSubsystem;

	struct FHeistogram* Heistogram;
	TQueue<uint64> FrameQueue;
};
