#include "EventSubsystem.h"
#include "CogCommon.h"
#include "EventGraphConditionData.h"
#include "EventGraphActionData.h"
#include "EventGraphAsset.h"
#include "EventGraphPluginSettings.h"

bool UEventSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const auto World = Cast<UWorld>(Outer);
	if (!World)
	{
		return false;
	}
	if (World->IsGameWorld())
	{
		if (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE)
		{
			const UEventGraphPluginSettings* Settings = GetDefault<UEventGraphPluginSettings>();

			if (!Settings)
			{
				return false;
			}

			const auto AllowedLevels = Settings->AllowedLevels;

			for (const auto& Level : AllowedLevels)
			{
				if (Level.IsValid() && !Level.ToSoftObjectPath().IsValid())
				{
					continue;
				}

				if (World->GetName() == Level.GetAssetName())
				{
					return true;
				}
			}
		}
	}
	return false;
}

void UEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UEventSubsystem::OnActorsInitialized);

	UE_LOG(LogTemp, Log, TEXT("UEventSubsystem Initialized"));
}

void UEventSubsystem::OnActorsInitialized(const FActorsInitializedParams& Params)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UEventSubsystem::InitEvents));
}



void UEventSubsystem::InitEvents()
{
	auto World = GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return;
	}
	const UEventGraphPluginSettings* Settings = GetDefault<UEventGraphPluginSettings>();
	if (!Settings)
	{
		return;
	}
	if (Settings->StartEventGraph.IsValid())
	{
		AddEventList(Settings->StartEventGraph.Get()); // already loaded
	}
	else
	{
		AddEventList(Settings->StartEventGraph.LoadSynchronous()); // load now
	}
}

void UEventSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bShouldCheckConditions)
	{
		CheckCurrentEventsState();
	}
}

void UEventSubsystem::Deinitialize()
{
	Super::Deinitialize();
	InvalidateTimers();
}

void UEventSubsystem::InvalidateTimers()
{
	for (FTimerHandle& Event : TimerHandles)
	{
		GetWorld()->GetTimerManager().ClearTimer(Event);
		Event.Invalidate();
	}
}

void UEventSubsystem::AddEventList(UEventGraphAsset* EventAsset)
{
	for (UEventGraphEventConditionData* Event : EventAsset->GraphData->Events)
	{
		AddEvent(Event);
	}
}

void UEventSubsystem::AddEvent(UEventGraphEventConditionData* EventData)
{
	if (!ensure(EventData))
	{
		return;
	}
	if (CurrentEvents.Contains(EventData) || CompletedEvents.Contains(EventData))
	{
		return;
	}
	CurrentEvents.AddUnique(EventData);
	EventData->SetupWorldContext(this);

	if (EventData->GetCondition() == nullptr)
	{
		LaunchEvent(EventData);
		return;
	}
	TArray<UEventGraphConditionData*> ConditionsList;
	EventData->GetCondition()->GetLeafs(ConditionsList);
	//Setup All the Value for the conditions
	for (UEventGraphConditionData* Condition : ConditionsList)
	{
		if (UEventGraphBoolConditionData* BoolCondition = Cast<UEventGraphBoolConditionData>(Condition))
		{
			if (!ConditionsMap.Contains(BoolCondition->GetKey()))
			{
				ConditionsMap.Add(BoolCondition->GetKey(), false);
			}
			BoolCondition->SetupWorldContext(this);
		}
	}
}

void UEventSubsystem::InvalidateEvent(class UEventGraphEventConditionData* EventData)
{
	if (!ensure(EventData)) { return; }
	if (!CurrentEvents.Contains(EventData))
	{
		if (!CompletedEvents.Contains(EventData))
		{
			CompletedEvents.AddUnique(EventData);
		}
		return;
	}
	CurrentEvents.Remove(EventData);
	CompletedEvents.AddUnique(EventData);
}

void UEventSubsystem::CallForConditionCheck()
{
	bShouldCheckConditions = true;
}

void UEventSubsystem::CheckCurrentEventsState()
{
	// #if ENABLE_COG
	//FCogDebug::StartEvent(this, "Events", "CheckConditions", true, 0, FColor::White);
	// #endif
	bShouldCheckConditions = false;
	TArray<UEventGraphEventConditionData*> CompletedEventsTemp;

	for (UEventGraphEventConditionData* Event : CurrentEvents)
	{
		check(Event)
		if (Event->IsValidated())
		{
			CompletedEventsTemp.Add(Event);
		}
	}

	for (UEventGraphEventConditionData* Event : CompletedEventsTemp)
	{
		check(Event)
		LaunchEvent(Event);
	}
}

bool UEventSubsystem::GetConditionValue(const FConditionKey& Key) const
{
	if (!ensure(ConditionsMap.Contains(Key))) { return false; }
	return ConditionsMap[Key];
}

void UEventSubsystem::SetConditionValue(const FConditionKey& Key, bool NewValue, bool UpdateEvent)
{
	if (!ConditionsMap.Contains(Key))
	{
		UE_LOG(LogTemp, Log, TEXT("Condition %s does not exist"), *Key.ConditionType->GetName());
		return;
	}
	ConditionsMap[Key] = NewValue;
	UE_LOG(LogTemp, Log, TEXT("%s has been set to : %s"), *Key.ConditionType->GetName(),
	       (NewValue ? TEXT("true") : TEXT("false")));

	bShouldCheckConditions = true;
}


void UEventSubsystem::LaunchEvent(class UEventGraphEventConditionData* EventData)
{
	check(EventData);

	for (UEventGraphActionData* EventAction : EventData->GetActions())
	{
		if (!ensure(EventAction)) { continue; }
		if (EventAction->ActionDelay == 0)
		{
			EventAction->LaunchEvent(this);
			continue;
		}
		FTimerDelegate EventLaunchTimerDel;
		EventLaunchTimerDel.BindUFunction(EventAction, "LaunchEvent", this);
		FTimerHandle EventLaunchTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(EventLaunchTimerHandle, EventLaunchTimerDel, EventAction->ActionDelay,
		                                       false);
		TimerHandles.AddUnique(EventLaunchTimerHandle);
		
	}

	CurrentEvents.Remove(EventData);
	CompletedEvents.AddUnique(EventData);

	bShouldCheckConditions = true;
}
