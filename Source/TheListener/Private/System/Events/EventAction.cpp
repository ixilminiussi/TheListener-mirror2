
#include "System/Events/EventAction.h"
#include "CogCommon.h"
#include "DialogueLineData.h"
#include "EventSubsystem.h"
#include "Asset/InkpotStoryAsset.h"
#include "GPE/Phone.h"
#include "GPE/Radio/StationDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LukaController.h"
#include "ProfilingDebugging/AssetMetadataTrace.h"
#include "System/Clues/ClueAsset.h"
#include "System/Clues/ClueSubsystem.h"

#include "System/Core/PlayGameMode.h"
#include "System/Dialogue/DialogueSubsystem.h"
#include "System/Frequency/FrequencySubsystem.h"

void UCustomAction::LaunchEvent(const UObject* InWorldContextObject)
{
	if (!ensure(InWorldContextObject)) { return; }

	UWorld* World = GEngine->GetWorldFromContextObjectChecked(InWorldContextObject);
	if (!ensure(World)) { return; }

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController)) { return; }

	ALukaController* LukaController = Cast<ALukaController>(PlayerController);
	LaunchEvent_(LukaController);
}

struct FActionKey UCustomAction::GenerateKey()
{
	return FActionKey(UCustomAction::StaticClass(),TEXT(""));
}

struct FActionKey UCustomAction::GetKey() const
{
	return UCustomAction::GenerateKey();
}

void URingPhoneAction::LaunchEvent(const UObject* InWorldContextObject)
{
	check(InWorldContextObject);

	if (UDialogueSubsystem* DialogueSubsystem = InWorldContextObject->GetWorld()->GetSubsystem<UDialogueSubsystem>();
		ensure(
			DialogueSubsystem))
	{
		DialogueSubsystem->GetPhone()->PushCall(DialogueTree, bIsImportant, RingDuration, bSkipQueue);
	}
}

struct FActionKey URingPhoneAction::GetKey() const
{
	return URingPhoneAction::GenerateKey(DialogueTree);
}

struct FActionKey URingPhoneAction::GenerateKey(class UDialogueTreeData* Dialogue)
{
	return FActionKey(URingPhoneAction::StaticClass(), Dialogue->GetName()); 
}

void UUnlockStationAction::LaunchEvent(const UObject* InWorldContextObject)
{
	check(InWorldContextObject);

	if (UFrequencySubsystem* FrequencySubsystem = InWorldContextObject->GetWorld()->GetSubsystem<UFrequencySubsystem>();
		ensure(
			FrequencySubsystem))
	{
		FrequencySubsystem->AddStation(Station);
	}
}

struct FActionKey UUnlockStationAction::GenerateKey(TObjectPtr<class UStationAsset> NewStation)
{
	return FActionKey(URingPhoneAction::StaticClass(), NewStation->GetName());
}

struct FActionKey UUnlockStationAction::GetKey() const
{
	return UUnlockStationAction::GenerateKey(Station);
}

void UUnlockPhoneAction::LaunchEvent(const UObject* InWorldContextObject)
{
	check(InWorldContextObject);

	if (UDialogueSubsystem* DialogueSubsystem = InWorldContextObject->GetWorld()->GetSubsystem<UDialogueSubsystem>();
		ensure(
			DialogueSubsystem))
	{
		DialogueSubsystem->GetPhone()->UnlockCall(NumberToCall, DialogueTree, AnswerDelay, bOverrideAll);
	}
}

struct FActionKey UUnlockPhoneAction::GetKey() const
{
	return UUnlockPhoneAction::GenerateKey(NumberToCall, DialogueTree);
}

struct FActionKey UUnlockPhoneAction::GenerateKey(TArray<int> Numbers, class UDialogueTreeData* DialogueTree)
{
	FString Data;
	for (int Number: Numbers)
	{
		Data.AppendInt(Number);
	}
	Data.Append("|");
	Data.Append(DialogueTree->GetName());
	return FActionKey(UUnlockPhoneAction::StaticClass(),Data);
}

void URemoveStationAction::LaunchEvent(const UObject* InWorldContextObject)
{
	check(InWorldContextObject);

	// #if ENABLE_COG
	FCogDebugEvent& CogEvent = FCogDebug::StartEvent(this, "Events", "Remove Station", true, 5, FColor::Red)
	                           .AddParam("Name", GetName())
	                           .AddParam("Delay", ActionDelay);

	if (Station)
	{
		CogEvent.AddParam("Station", Station->GetName());
	}
	else
	{
		CogEvent.AddParam("Station", FString("NaN"));
	}
	// #endif

	if (const UFrequencySubsystem* FrequencySubsystem = InWorldContextObject->GetWorld()->GetSubsystem<
		UFrequencySubsystem>(); ensure(
		FrequencySubsystem))
	{
		FrequencySubsystem->DestroyStation(Station);
	}
}

struct FActionKey URemoveStationAction::GetKey() const
{
	return URemoveStationAction::GenerateKey(Station);
}

struct FActionKey URemoveStationAction::GenerateKey(TObjectPtr<class UStationAsset> RemoveStation)
{
	return FActionKey(URingPhoneAction::StaticClass(), RemoveStation->GetName());
}

void URemoveEventAction::LaunchEvent(const UObject* InWorldContextObject)
{
	check(InWorldContextObject);

	// #if ENABLE_COG
	FCogDebugEvent& CogEvent = FCogDebug::StartEvent(this, "Events", "Remove Event", true, 6, FColor::Red)
	                           .AddParam("Name", GetName())
	                           .AddParam("Delay", ActionDelay);

}

struct FActionKey URemoveEventAction::GenerateKey(FName EventToRemove)
{
	return FActionKey(URemoveEventAction::StaticClass(),EventToRemove.ToString());
}

struct FActionKey URemoveEventAction::GetKey() const
{
	return URemoveEventAction::GenerateKey(EventNameToRemove);
}

void UGiveClueAction::LaunchEvent(const UObject* InWorldContextObject)
{
	check(InWorldContextObject);

	// #if ENABLE_COG
	FCogDebugEvent& CogEvent = FCogDebug::StartEvent(this, "Events", "Give Clue", true, 7, FColor::Yellow)
	                           .AddParam("Name", GetName())
	                           .AddParam("Delay", ActionDelay);

	if (Clue)
	{
		CogEvent.AddParam("Event", Clue->GetName());
	}
	else
	{
		CogEvent.AddParam("Event", FString("NaN"));
	}
	// #endif

	if (UClueSubsystem* ClueSubsystem = InWorldContextObject->GetWorld()->GetSubsystem<UClueSubsystem>();
		ensure(ClueSubsystem))
	{
		ClueSubsystem->GiveClue(Clue);
	}
}

struct FActionKey UGiveClueAction::GenerateKey(TObjectPtr<class UClueAsset> NewClue)
{
	return FActionKey(URingPhoneAction::StaticClass(), NewClue->GetName());
}

struct FActionKey UGiveClueAction::GetKey() const
{
	return UGiveClueAction::GenerateKey(Clue);
}

void URemoveClueAction::LaunchEvent(const UObject* InWorldContextObject)
{
	check(InWorldContextObject);

	// #if ENABLE_COG
	FCogDebugEvent& CogEvent = FCogDebug::StartEvent(this, "Events", "Remove Clue", true, 8, FColor::Yellow)
	                           .AddParam("Name", GetName())
	                           .AddParam("Delay", ActionDelay);

	if (Clue)
	{
		CogEvent.AddParam("Event", Clue->GetName());
	}
	else
	{
		CogEvent.AddParam("Event", FString("NaN"));
	}
	// #endif

	if (UClueSubsystem* ClueSubsystem = InWorldContextObject->GetWorld()->GetSubsystem<UClueSubsystem>();
		ensure(ClueSubsystem))
	{
		ClueSubsystem->RemoveClue(Clue);
	}
}

struct FActionKey URemoveClueAction::GetKey() const
{
	return URemoveClueAction::GenerateKey(Clue);
}

struct FActionKey URemoveClueAction::GenerateKey(TObjectPtr<class UClueAsset> RemoveClue)
{
	return FActionKey(URingPhoneAction::StaticClass(), RemoveClue->GetName());
}
