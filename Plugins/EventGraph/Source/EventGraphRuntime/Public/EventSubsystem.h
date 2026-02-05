#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EventGraphConditionData.h"
#include "EventSubsystem.generated.h"


USTRUCT(Blueprintable)
struct FConditionKey
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<UEventGraphBoolConditionData> ConditionType = nullptr;
	UPROPERTY()
	FString Data{""};
	UPROPERTY()
	float TimeRequired{0.f}; // for timed event
	inline bool operator==(const FConditionKey& Other) const
	{
		return ConditionType == Other.ConditionType && Data == Other.Data && TimeRequired == Other.TimeRequired;
	}

};

USTRUCT(Blueprintable)
struct FActionKey
{
	GENERATED_BODY()

	UPROPERTY()
	UClass* ActionType= nullptr;
	UPROPERTY()
	FString Data{""};
//Test Update
	bool operator==(const FActionKey& Other) const
	{
		return ActionType == Other.ActionType && Data == Other.Data;
	}
	
};

FORCEINLINE uint32 GetTypeHash(const FConditionKey& Key)
{
	uint32 Hash = GetTypeHash(Key.ConditionType);
	Hash = HashCombine(Hash, GetTypeHash(Key.Data));
	Hash = HashCombine(Hash, GetTypeHash(Key.TimeRequired));
	return Hash;
}

UCLASS()
class EVENTGRAPHRUNTIME_API UEventSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void OnActorsInitialized(const FActorsInitializedParams& Params);
	void OnPreWorldInitialization(UWorld* World, const UWorld::InitializationValues);
	void OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues);
	void InitEvents();
	virtual void Deinitialize() override;
	void InvalidateTimers();
	UFUNCTION(BlueprintCallable)
	void PauseTimers();
	void UnPauseTimers();
	bool ShouldLoadEvents();

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { return GetStatID(); }

	void AddEventList(class UEventGraphAsset* EventDataAsset);
	void AddEvent(class UEventGraphEventConditionData* EventDataAsset);

	void InvalidateEvent(class UEventGraphEventConditionData* EventDataAsset);

	void CallForConditionCheck();

	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool GetConditionValue(const FConditionKey& Key) const;

	UFUNCTION(BlueprintCallable)
	void SetConditionValue(const FConditionKey& Key, bool bNewValue, bool UpdateEvent = true);

	//It is recommended to reload the level for the reset to be fully complete
	UFUNCTION(BlueprintCallable)
	void ResetSubsystem(bool ForceLoad);
	
	friend class FCogStoryWindow_Events;

protected:
	void CheckCurrentEventsState();

	UPROPERTY()
	TMap<FConditionKey, bool> ConditionsMap;

	void LaunchEvent(class UEventGraphEventConditionData* EventData);

	UPROPERTY()
	TArray<TObjectPtr<class UEventGraphEventConditionData>> CurrentEvents;
	UPROPERTY()
	TArray<TObjectPtr<class UEventGraphEventConditionData>> CompletedEvents;

	UPROPERTY()
	bool bShouldCheckConditions = false;

	UPROPERTY()
	TArray<FTimerHandle> TimerHandles;

	unsigned int DayCounter = 0;

	UPROPERTY()
	bool bIsLoaded = false;
};
