#include "System/Events/EventCondition.h"

#include "EventSubsystem.h"
#include "GPE/Obji.h"
#include "GPE/Toy.h"
#include "GPE/Radio/StationDataAsset.h"
#include "Player/LukaController.h"
#include "UI/InspectWidget.h"

void UCustomCondition::SetupWorldContext(UObject* InWorldContextObject)
{
	WorldContextObject = InWorldContextObject;

	Super::SetupWorldContext(InWorldContextObject);

	if (!ensure(InWorldContextObject)) { return; }

	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(InWorldContextObject);
	if (!ensure(World)) { return; }

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController)) { return; }

	ALukaController* LukaController = Cast<ALukaController>(PlayerController);
	SetupContext_(LukaController);
}

struct FConditionKey UDebugCondition::GenerateKey(const FString& Name)
{
	return FConditionKey(StaticClass(), Name);
}

struct FConditionKey UDebugCondition::GetKey() const
{
	return GenerateKey(ConditionName);
}

struct FConditionKey UToyPossessedCondition::GetKey() const
{
	return GenerateKey(ToyClass);
}

struct FConditionKey UInkpotCondition::GetKey() const
{
	return GenerateKey(InkpotTagName);
}

struct FConditionKey UInkpotCondition::GenerateKey(const FString& TagName)
{
	return FConditionKey(StaticClass(), TagName);
}

struct FConditionKey UCustomCondition::GenerateKey(const FString& Name)
{
	return FConditionKey(StaticClass(), Name);
}

struct FConditionKey UCustomCondition::GetKey() const
{
	return FConditionKey(GetClass(), ConditionName);
}

struct FConditionKey UObjiHeldCondition::GetKey() const
{
	return GenerateKey(ObjiClass);
}

struct FConditionKey UObjiHeldCondition::GenerateKey(const UClass* ObjiClass)
{
	if (ensure(ObjiClass))
	{
		return FConditionKey(StaticClass(), ObjiClass->GetName());
	}
	return FConditionKey(StaticClass(), "Invalid Obji");
}

struct FConditionKey UStationListenedCondition::GetKey() const
{
	return GenerateKey(Station);
}

struct FConditionKey UStationListenedCondition::GenerateKey(const UStationAsset* Station)
{
	if (ensure(Station))
	{
		return FConditionKey(StaticClass(), Station->GetName());
	}
	return FConditionKey(StaticClass(), "Invalid Station");
}

struct FConditionKey UActiveTimeCondition::GetKey() const
{
	return GenerateKey(ChronoName, TimeRequired);
}

struct FConditionKey UActiveTimeCondition::GenerateKey(const FString& CName, const float Time)
{
	return FConditionKey(StaticClass(), CName, Time);
}

struct FConditionKey UToyPossessedCondition::GenerateKey(const UClass* ToyClass)
{
	if (ensure(ToyClass))
	{
		return FConditionKey(StaticClass(), ToyClass->GetName());
	}
	return FConditionKey(StaticClass(), "Invalid ToyClass");
}

struct FConditionKey UStationFinishedCondition::GetKey() const
{
	return GenerateKey(Station);
}

struct FConditionKey UStationFinishedCondition::GenerateKey(const UStationAsset* Station)
{
	if (ensure(Station))
	{
		return FConditionKey(StaticClass(), Station->GetName());
	}
	return FConditionKey(StaticClass(), "Invalid Station");
}

void UActiveTimeCondition::TimerFinish()
{
	//Update Time Condition
	SetConditionMet(true);

	check(WorldContextObject)
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (!ensure(World)) { return; }

	World->GetTimerManager().ClearTimer(ActiveTimeHandle);
}

void UActiveTimeCondition::SetupWorldContext(UObject* InWorldContextObject)
{
	Super::SetupWorldContext(InWorldContextObject);

	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (!ensure(World)) { return; }

	World->GetTimerManager().SetTimer(ActiveTimeHandle, this, &UActiveTimeCondition::TimerFinish, TimeRequired, false);
}

struct FConditionKey USelectorHighlightCondition::GenerateKey(UClass* InspectWidgetClass, FString Name)
{
	FString Data = InspectWidgetClass->GetName();
	Data.Append(Name);
	return FConditionKey(USelectorHighlightCondition::StaticClass(), Data);
}

struct FConditionKey USelectorHighlightCondition::GetKey() const
{
	return USelectorHighlightCondition::GenerateKey(WidgetInspected,SelectorName);
}

void UActiveTimeCondition::BeginDestroy()
{
	Super::BeginDestroy();
	
	if (!WorldContextObject) { return; }
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (!ensure(World)) { return; }

	World->GetTimerManager().ClearTimer(ActiveTimeHandle);
}
