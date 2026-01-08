#include "EventGraphConditionData.h"

#include "EventSubsystem.h"

bool UNotCondition::IsValidated()
{
	if (ensure(Conditions[0]))
	{
		return !(Conditions[0]->IsValidated());
	}

	return false;
}

FText UThresholdCondition::GetNodeTitle() const
{
	FString NodeTitle = TEXT("Threshold \n (");
	NodeTitle.AppendInt(Threshold);
	NodeTitle.Append(")");

	return FText::FromString(NodeTitle);
}

void UEventGraphConditionData::ClearConnectedData()
{
}

#if WITH_EDITOR
void UEventGraphConditionData::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	OnPropertyChanged.Broadcast();
}
#endif
void UEventGraphLogicConditionData::GetLeafs(TArray<UEventGraphConditionData*>& EventConditions)
{
	for (UEventGraphConditionData* Condition : Conditions)
	{
		EventConditions.Add(Condition);
	}
}

void UEventGraphLogicConditionData::ClearConnectedData()
{
	Conditions.Empty();
}

void UEventGraphLogicConditionData::SetConditions(TArray<UEventGraphConditionData*>& CList)
{
	Conditions.Empty();
	Conditions = TArray<UEventGraphConditionData*>(CList);
}

bool UAndCondition::IsValidated()
{
	if (!IsConditionEnabled){return true;}
	for (UEventGraphConditionData* Condition : Conditions)
	{
		if (ensure(Condition))
		{
			if (!Condition->IsValidated())
			{
				return false;
			}
		}
	}
	return true;
}

bool UOrCondition::IsValidated()
{
	if (!IsConditionEnabled){return true;}
	for (UEventGraphConditionData* Condition : Conditions)
	{
		if (ensure(Condition))
		{
			if (Condition->IsValidated())
			{
				return true;
			}
		}
	}
	return false;
}

bool UThresholdCondition::IsValidated()
{
	if (!IsConditionEnabled){return true;}
	int CurrentTracker = 0;
	for (UEventGraphConditionData* Condition : Conditions)
	{
		if (ensure(Condition))
		{
			if (Condition->IsValidated())
			{
				CurrentTracker++;
			}
		}
	}
	return CurrentTracker >= Threshold;
}

bool UEventGraphBoolConditionData::IsValidated()
{
	check(WorldContextObject);
	if (!IsConditionEnabled){return true;}
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (!ensure(World)) { return false; }

	if (!ensure(World->GetSubsystem<UEventSubsystem>())) { return false; }
	return World->GetSubsystem<UEventSubsystem>()->GetConditionValue(GetKey());
}

void UEventGraphBoolConditionData::GetLeafs(TArray<UEventGraphConditionData*>& EventConditions)
{
	EventConditions.Add(this);
}

void UEventGraphBoolConditionData::SetupWorldContext(UObject* InWorldContextObject)
{
	WorldContextObject = InWorldContextObject;
}

FConditionKey UEventGraphBoolConditionData::GetKey() const
{
	FConditionKey Key{};
	Key.ConditionType = GetClass();

	return Key;
}

void UEventGraphBoolConditionData::SetConditionMet(const bool bIsMet) const
{
	check(WorldContextObject);

	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (!ensure(World)) { return; }

	if (!ensure(World->GetSubsystem<UEventSubsystem>())) { return; }
	World->GetSubsystem<UEventSubsystem>()->SetConditionValue(GetKey(), bIsMet, true);
}

void UEventGraphEventConditionData::ClearConnectedData()
{
	Condition = nullptr;
}

bool UEventGraphEventConditionData::IsValidated()
{
	if (!IsConditionEnabled){return true;}
	if (!Super::IsValidated())
	{
		if (!Condition) { return true; }
		return Condition->IsValidated();
	}
	return true;
}

struct FConditionKey UEventGraphEventConditionData::GenerateKey(const UEventGraphEventConditionData* Event)
{
	return FConditionKey(StaticClass(), Event->GetName());
}

struct FConditionKey UEventGraphEventConditionData::GetKey() const
{
	return GenerateKey(this);
}
