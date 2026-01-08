// Fill out your copyright notice in the Description page of Project Settings.

#define LogicColor FLinearColor(0.768f,0.0f,0.004f,1.0f)
#define ConditionColor FLinearColor(0.0f,0.646f,0.104f,1.0f)
#define EventColor FLinearColor(0.037f,0.776f,1.0f,1.0f)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EventGraphConditionData.generated.h"

DECLARE_MULTICAST_DELEGATE(FPropertyChangedDelegate);

UENUM()
enum EEventGraphConditionType : int8
{
	Event = 0,
	Condition = 1,
	Logic = 2
};

/**
 * 
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced)
class EVENTGRAPHRUNTIME_API UEventGraphConditionData : public UObject
{
	GENERATED_BODY()

public:
	FLinearColor GetTrueNodeColor() {return IsConditionEnabled ?  GetNodeColor() :FLinearColor(0.1f,0.1f,0.1f,1.0f);}
	virtual FLinearColor GetNodeColor() const {return FLinearColor(0.25f,0.63f,0.17f);}
	virtual FText GetNodeTitle() const {return FText::FromString("Condition Node");}
	virtual FText GetNodeDescription() const {return FText::FromString("");}

	virtual void ClearConnectedData();
	
	virtual bool IsValidated() PURE_VIRTUAL( UEventGraphConditionData::IsConditionValidated, return false;);

	virtual void GetLeafs(TArray<UEventGraphConditionData*>&) PURE_VIRTUAL( UEventGraphConditionData::getConditions, return void(););

	static struct FActionKey GetConditionKey();

	

#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	FPropertyChangedDelegate OnPropertyChanged;
#endif
	virtual EEventGraphConditionType GetType() const {return Condition;}

	UPROPERTY()
	bool IsConditionEnabled = true;
};

UCLASS(Abstract)
class EVENTGRAPHRUNTIME_API UEventGraphLogicConditionData : public UEventGraphConditionData
{
	GENERATED_BODY()

public:
	virtual FLinearColor GetNodeColor() const {return LogicColor;}
	
	virtual bool CanHaveMultipleInputPins() const {return true;}

	virtual void GetLeafs(TArray<UEventGraphConditionData*>&) override;

	static struct FActionKey GetConditionKey();

	virtual void ClearConnectedData() override;
	
	void SetConditions(TArray<UEventGraphConditionData*>&);

	virtual EEventGraphConditionType GetType() const override {return Logic;}

protected:
	UPROPERTY()
	TArray< UEventGraphConditionData*> Conditions{};
};

UCLASS(meta=(DisplayName="And"))
class EVENTGRAPHRUNTIME_API UAndCondition : public  UEventGraphLogicConditionData
{
	GENERATED_BODY()

public:
	virtual FText GetNodeTitle() const {return FText::FromString("And");}
	virtual FText GetNodeDescription() const {return FText::FromString("");}
	
	virtual bool IsValidated() override;
};

UCLASS(meta=(DisplayName="Or"))
class EVENTGRAPHRUNTIME_API UOrCondition : public  UEventGraphLogicConditionData
{
	GENERATED_BODY()

public:
	virtual FText GetNodeTitle() const {return FText::FromString("Or");}
	virtual FText GetNodeDescription() const {return FText::FromString("");}
	
	virtual bool IsValidated() override;
};

UCLASS(meta=(DisplayName="Not"))
class EVENTGRAPHRUNTIME_API UNotCondition : public  UEventGraphLogicConditionData
{
	GENERATED_BODY()

public:
	virtual FText GetNodeTitle() const {return FText::FromString("Not");}
	virtual FText GetNodeDescription() const {return FText::FromString("");}
	virtual bool CanHaveMultipleInputPins() const override {return false;}
	virtual bool IsValidated() override;
};

UCLASS(meta=(DisplayName="Threshold"))
class EVENTGRAPHRUNTIME_API UThresholdCondition : public  UEventGraphLogicConditionData
{
	GENERATED_BODY()

public:
virtual FText GetNodeTitle() const override ;
	virtual FText GetNodeDescription() const {return FText::FromString("");}
	virtual bool IsValidated() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Threshold = 1;
};

UCLASS()
class EVENTGRAPHRUNTIME_API UEventGraphBoolConditionData : public UEventGraphConditionData
{
	GENERATED_BODY()

public:
	virtual bool IsValidated() override;

	virtual void GetLeafs(TArray< UEventGraphConditionData*>&) override;

	virtual void SetupWorldContext(UObject* InWorldContextObject);

	virtual struct FConditionKey GetKey() const;

	virtual EEventGraphConditionType GetType() const override {return Condition;}

protected:
	UFUNCTION(BlueprintCallable, Category = "Condition")
	void SetConditionMet(bool bIsMet) const;

	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject;
};

UCLASS()
class EVENTGRAPHRUNTIME_API UEventGraphEventConditionData : public UEventGraphBoolConditionData 
{
	GENERATED_BODY()

	virtual FLinearColor GetNodeColor() const {return EventColor;}//NodeColor;}
	virtual FText GetNodeTitle() const {return EventName.IsNone() ? FText::FromString("Event") : FText::FromName(EventName); }
	virtual FText GetNodeDescription() const {return FText::FromString("");}

public:
	virtual struct FConditionKey GetKey() const override;
	static struct FConditionKey GenerateKey(const UEventGraphEventConditionData* Event);
	TArray<class UEventGraphActionData*> GetActions() const {return Actions;}

	virtual void ClearConnectedData() override;

	virtual EEventGraphConditionType GetType() const override {return Event;}

	virtual bool IsValidated() override;;

	FName GetEventName() const {return EventName;}
	FString GetEventNameAsString() const {return EventName.ToString();}

	void SetCondition(UEventGraphConditionData* newCondition) {Condition = newCondition;}
	class UEventGraphConditionData* GetCondition() {return Condition;}

protected:
	UPROPERTY()
	class UEventGraphConditionData* Condition;

	/*UPROPERTY(EditAnywhere)
	FColor NodeColor;*/
	
	UPROPERTY(EditAnywhere)
	FName EventName;

	UPROPERTY(EditAnywhere)
	TArray<class UEventGraphActionData*> Actions{};
};

UCLASS(Abstract)
class EVENTGRAPHRUNTIME_API UEventGraphBaseConditionData : public UEventGraphBoolConditionData
{
	GENERATED_BODY()

public:
	virtual FLinearColor GetNodeColor() const {return ConditionColor;}
};