#include "System/Debug/CogConfigurationSubsystem.h"

#include "CogCommon.h"

#if ENABLE_COG
#include "CogEngineWindow_BuildInfo.h"
#include "CogEngineWindow_CollisionViewer.h"
#include "CogEngineWindow_CommandBindings.h"
#include "CogEngineWindow_Console.h"
#include "CogEngineWindow_DebugSettings.h"
#include "CogEngineWindow_ImGui.h"
#include "CogEngineWindow_Plots.h"
#include "CogEngineWindow_Selection.h"
#include "CogEngineWindow_Stats.h"
#include "CogEngineWindow_TimeScale.h"
#include "CogEngineWindow_Transform.h"
#include "CogInputWindow_Actions.h"
#include "CogInputWindow_Gamepad.h"
#include "CogSubsystem.h"
#include "System/Debug/CogStoryWindow_Events.h"
#include "System/Debug/CogGameplayWindow_Utilities.h"
#endif

bool UCogConfigurationSubsystem::ShouldCreateSubsystem(UObject* Outer) const
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

void UCogConfigurationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if ENABLE_COG
	CogSubsystem = Collection.InitializeDependency<UCogSubsystem>();
	Collection.InitializeDependency<UCogConfigurationSubsystem>();
#endif
}

void UCogConfigurationSubsystem::PostInitialize()
{
	Super::PostInitialize();

#if ENABLE_COG
	UCogSubsystem* CogSubsystemPtr = Cast<UCogSubsystem>(CogSubsystem.Get());

	if (IsValid(CogSubsystemPtr) == false)
	{
		return;
	}

	// Cog::AddAllWindows(*CogSubsystemPtr);

	CogSubsystemPtr->AddWindow<FCogEngineWindow_BuildInfo>("Engine.BuildInfo");
	CogSubsystemPtr->AddWindow<FCogEngineWindow_CollisionViewer>("Engine.CollisionViewer");
	CogSubsystemPtr->AddWindow<FCogEngineWindow_CommandBindings>("Engine.CommandBindings");
	CogSubsystemPtr->AddWindow<FCogEngineWindow_Console>("Engine.Console");
	CogSubsystemPtr->AddWindow<FCogEngineWindow_DebugSettings>("Engine.DebugSettings");
	CogSubsystemPtr->AddWindow<FCogEngineWindow_ImGui>("Engine.ImGui");
	CogSubsystemPtr->AddWindow<FCogEngineWindow_Selection>("Engine.DebugSettings");
	CogSubsystemPtr->AddWindow<FCogEngineWindow_TimeScale>("Engine.TimeScale");
	CogSubsystemPtr->AddWindow<FCogEngineWindow_Transform>("Engine.Transform");
	CogSubsystemPtr->AddWindow<FCogEngineWindow_Stats>("Engine.Stats");
	CogSubsystemPtr->AddWindow<FCogEngineWindow_Plots>("Engine.Plots");

	CogSubsystemPtr->AddWindow<FCogInputWindow_Actions>("Input.Actions");
	CogSubsystemPtr->AddWindow<FCogInputWindow_Gamepad>("Input.Gamepad");

	if (GetWorld()->GetSubsystem<UEventSubsystem>())
	{
		CogSubsystemPtr->AddWindow<FCogStoryWindow_Events>("Story.Event");
	}
	CogSubsystemPtr->AddWindow<FCogGameplayWindow_Utilities>("Gameplay.Utilities");
#endif
}
