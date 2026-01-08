// Fill out your copyright notice in the Description page of Project Settings.


#include "EventGraphActionData.h"

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

struct FActionKey UDebugEventGraphActionData::GenerateKey(FString DebugString)
{
	return FActionKey(UEventGraphActionData::StaticClass(),DebugString);
}

struct FActionKey UDebugEventGraphActionData::GetKey() const
{
	return UDebugEventGraphActionData::GenerateKey(DebugString);
}
