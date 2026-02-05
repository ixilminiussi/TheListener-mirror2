// Fill out your copyright notice in the Description page of Project Settings.


#include "EventGraphActionData.h"
#include "EventGraphAsset.h"
#include "EventSubsystem.h"

struct FActionKey UEventGraphActionData::GenerateKey()
{
	return FActionKey(UEventGraphActionData::StaticClass(),"");
}

struct FActionKey UEventGraphActionData::GetKey() const
{
	return UEventGraphActionData::GenerateKey();
}

void UDebugEventGraphActionData::LaunchEvent(const UObject* InWorldContextObject)
{
	UE_LOG(LogTemp, Display, TEXT("%s"), *DebugString);
}

void ULoadEventAction::LaunchEvent(const UObject* InWorldContextObject)
{
	//I know i could just take the InWorldContextObject but just in case, the smallest case that this gets changed i will not...
	UEventSubsystem* Subsystem = InWorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UEventSubsystem>();
	if (ensure(Subsystem))
	{
		Subsystem->AddEventList(EventAsset);
	}
}

struct FActionKey ULoadEventAction::GenerateKey(UEventGraphAsset* DayToLoad)
{
	return FActionKey(ULoadEventAction::StaticClass(),DayToLoad->GetName());
}

struct FActionKey ULoadEventAction::GetKey() const
{
	return ULoadEventAction::GenerateKey(EventAsset);
}

struct FActionKey UDebugEventGraphActionData::GenerateKey(FString DebugString)
{
	return FActionKey(UEventGraphActionData::StaticClass(),DebugString);
}

struct FActionKey UDebugEventGraphActionData::GetKey() const
{
	return UDebugEventGraphActionData::GenerateKey(DebugString);
}
