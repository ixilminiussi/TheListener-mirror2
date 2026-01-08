#include "System/Debug/CogPlottingSubsystem.h"

#include "CogCommon.h"

#if ENABLE_COG
#include "CogDebug.h"
#include "CogSubsystem.h"
#include "System/Debug/CogConfigurationSubsystem.h"
#include "System/Debug/heistogram.h"
#endif

bool UCogPlottingSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (Super::ShouldCreateSubsystem(Outer) == false)
	{
		return false;
	}

#if ENABLE_COG
	return true;
#else
	return false;
#endif
}

void UCogPlottingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if ENABLE_COG
	CogSubsystem = Collection.InitializeDependency<UCogSubsystem>();
	Collection.InitializeDependency<UCogConfigurationSubsystem>();
	Heistogram = heistogram_create();
#endif
}

void UCogPlottingSubsystem::Deinitialize()
{
	Super::Deinitialize();

#if ENABLE_COG
	heistogram_free(Heistogram);
	Heistogram = nullptr;
#endif
}

void UCogPlottingSubsystem::Tick(const float DeltaTime)
{
#if ENABLE_COG
	extern ENGINE_API float GAverageFPS;
	FCogDebug::Plot(this, "Average FPS", GAverageFPS);


	// calculate bottom 1% and .1% of last 6000 frames using heistogram
	const uint64 RawFPS = (DeltaTime <= 0.f) ? 0u : static_cast<uint64>(1.0f / DeltaTime);
	FrameQueue.Enqueue(RawFPS);
	heistogram_add(Heistogram, RawFPS);

	constexpr int MaxQueueSize = 3000;
	if (heistogram_count(Heistogram) > MaxQueueSize)
	{
		heistogram_remove(Heistogram, *FrameQueue.Peek());
		FrameQueue.Pop();
	}

	FCogDebug::Plot(this, "Bottom 1% FPS", heistogram_percentile(Heistogram, 1.0f));
	FCogDebug::Plot(this, "Bottom .1% FPS", heistogram_percentile(Heistogram, 0.1f));
#endif
}
