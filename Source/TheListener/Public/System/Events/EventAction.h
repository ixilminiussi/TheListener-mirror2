#pragma once

#include "CoreMinimal.h"
#include "EventGraphRuntime/Public/EventGraphActionData.h"
#include "EventAction.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLaunch);

UCLASS(Blueprintable)
class THELISTENER_API UCustomAction : public UEventGraphActionData
{
	GENERATED_BODY()

public:
	virtual void LaunchEvent(const UObject* InWorldContextObject) override;

	virtual struct FActionKey GetKey() const override;
	static struct FActionKey GenerateKey();

	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	void LaunchEvent_(ALukaController* LukaController);

protected:
	UPROPERTY(Category = "Action", EditAnywhere, BlueprintReadWrite)
	FString Text{};
};

UCLASS()
class THELISTENER_API URingPhoneAction : public UEventGraphActionData
{
	GENERATED_BODY()

public:
	virtual void LaunchEvent(const UObject* InWorldContextObject) override;

	virtual struct FActionKey GetKey() const override;
	static struct FActionKey GenerateKey(class UDialogueTreeData* Story);
	
protected:
	UPROPERTY(Category = "Action", EditAnywhere)
	class UDialogueTreeData* DialogueTree = nullptr;

	UPROPERTY(Category = "Action", EditAnywhere)
	bool bIsImportant = false;
	
	UPROPERTY(Category = "Action", EditAnywhere)
	bool bSkipQueue = false;
	
	UPROPERTY(Category = "Action", EditAnywhere)
	float RingDuration = 10.f;
};

UCLASS()
class THELISTENER_API UUnlockPhoneAction : public UEventGraphActionData
{
	GENERATED_BODY()

public:
	virtual void LaunchEvent(const UObject* InWorldContextObject) override;

	virtual struct FActionKey GetKey() const override;
	static struct FActionKey GenerateKey(TArray<int> Number, class UDialogueTreeData* DialogueTree);

protected:
	/** Number Should be no more or less than 6 digits otherwise they will never be found by the phone **/
	UPROPERTY(Category = "Action", EditAnywhere)
	TObjectPtr<class UDialogueTreeData> DialogueTree;
	
	UPROPERTY(Category = "Action", EditAnywhere)
	TArray<int> NumberToCall;
	
	UPROPERTY(Category = "Action", EditAnywhere)
	float AnswerDelay = 2.f;
	
	UPROPERTY(Category = "Action", EditAnywhere)
	bool bOverrideAll = false;
};

UCLASS()
class THELISTENER_API UUnlockStationAction : public UEventGraphActionData
{
	GENERATED_BODY()

public:
	virtual void LaunchEvent(const UObject* InWorldContextObject) override;

	virtual struct FActionKey GetKey() const override;
	static struct FActionKey GenerateKey(TObjectPtr<class UStationAsset> NewStation);

protected:
	UPROPERTY(Category = "Action", EditAnywhere)
	TObjectPtr<class UStationAsset> Station = nullptr;
};

UCLASS()
class THELISTENER_API URemoveStationAction : public UEventGraphActionData
{
	GENERATED_BODY()

public:
	virtual void LaunchEvent(const UObject* InWorldContextObject) override;

	virtual struct FActionKey GetKey() const override;
	static struct FActionKey GenerateKey(TObjectPtr<class UStationAsset> RemoveStation);

protected:
	UPROPERTY(Category = "Action", EditAnywhere)
	TObjectPtr<class UStationAsset> Station = nullptr;
};

UCLASS()
class THELISTENER_API URemoveEventAction : public UEventGraphActionData
{
	GENERATED_BODY()

public:
	virtual void LaunchEvent(const UObject* InWorldContextObject) override;

	virtual struct FActionKey GetKey() const override;
	static struct FActionKey GenerateKey(FName EventToRemove);

protected:
	UPROPERTY(Category = "Action", EditAnywhere)
	FName EventNameToRemove = "";
};

UCLASS()
class THELISTENER_API UGiveClueAction : public UEventGraphActionData
{
	GENERATED_BODY()

public:
	virtual void LaunchEvent(const UObject* InWorldContextObject) override;

	virtual struct FActionKey GetKey() const override;
	static struct FActionKey GenerateKey(TObjectPtr<class UClueAsset> NewClue);

protected:
	UPROPERTY(Category = "Action", EditAnywhere)
	TObjectPtr<class UClueAsset> Clue = nullptr;
};

UCLASS()
class THELISTENER_API URemoveClueAction : public UEventGraphActionData
{
	GENERATED_BODY()

public:
	virtual void LaunchEvent(const UObject* InWorldContextObject) override;

	virtual struct FActionKey GetKey() const override;
	static struct FActionKey GenerateKey(TObjectPtr<class UClueAsset> RemoveClue);

protected:
	UPROPERTY(Category = "Action", EditAnywhere)
	TObjectPtr<class UClueAsset> Clue = nullptr;
};
