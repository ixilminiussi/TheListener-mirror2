#pragma once

#include "CoreMinimal.h"
#include "EventGraphRuntime/Public/EventGraphConditionData.h"
#include "EventCondition.generated.h"


UCLASS(Blueprintable, Abstract)
class THELISTENER_API UCustomCondition : public UEventGraphBaseConditionData
{
	GENERATED_BODY()

public:
	virtual void SetupWorldContext(UObject* InWorldContextObject) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Condition")
	void SetupContext_(class ALukaController* LukaController);

	virtual FText GetNodeTitle() const override { return GetNodeTitle_(); }
	virtual FText GetNodeDescription() const override { return GetNodeDesc_(); }
	virtual FLinearColor GetNodeColor() const override { return GetNodeColor_(Super::GetNodeColor()); }
	UFUNCTION(BlueprintImplementableEvent, Category = "Condition")
	FText GetNodeTitle_() const;
	UFUNCTION(BlueprintImplementableEvent, Category = "Condition")
	FText GetNodeDesc_() const;
	UFUNCTION(BlueprintImplementableEvent, Category = "Condition")
	FLinearColor GetNodeColor_(FLinearColor DefaultColor) const;

	virtual struct FConditionKey GetKey() const override;
	static struct FConditionKey GenerateKey(const FString& Name);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FString ConditionName = "Name";
};

UCLASS(DisplayName = "Debug", meta = (ToolTip = "Condition Used for debug"))
class THELISTENER_API UDebugCondition : public UEventGraphBaseConditionData
{
	GENERATED_BODY()

public:
	virtual struct FConditionKey GetKey() const override;
	static struct FConditionKey GenerateKey(const FString& Name);

	virtual FText GetNodeTitle() const override { return FText::FromString("Debug Condition"); }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FString ConditionName = "Name";
};

UCLASS(DisplayName = "Has Heard/Said", meta = (ToolTip = "is this Inkpot Tag registered ?"))
class THELISTENER_API UInkpotCondition : public UEventGraphBaseConditionData
{
	GENERATED_BODY()

public:
	virtual struct FConditionKey GetKey() const override;
	static struct FConditionKey GenerateKey(const FString& TagName);

	virtual FText GetNodeTitle() const override {return FText::FromString("Inkpot Tag");}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FString InkpotTagName = "Name";
};

UCLASS(DisplayName = "Is Toy Possessed", meta = (ToolTip = "Is the selected toy currently possessed"))
class THELISTENER_API UToyPossessedCondition : public UEventGraphBaseConditionData
{
	GENERATED_BODY()

public:
	virtual struct FConditionKey GetKey() const override;
	static struct FConditionKey GenerateKey(const UClass* ToyClass);

	virtual FText GetNodeTitle() const override {return FText::FromString("Toy Possessed");}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	TSubclassOf<class AToy> ToyClass;
};

UCLASS(DisplayName = "Is Obji Held", meta = (ToolTip = "Is the selected obji currently held"))
class THELISTENER_API UObjiHeldCondition : public UEventGraphBaseConditionData
{
	GENERATED_BODY()

public:
	virtual struct FConditionKey GetKey() const override;
	static struct FConditionKey GenerateKey(const UClass* ObjiClass);

	virtual FText GetNodeTitle() const override {return FText::FromString("Obji Held");}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	TSubclassOf<class AObji> ObjiClass;
};

UCLASS(DisplayName = "Has Heard Station", meta = (ToolTip = "Did the player listened to this station"))
class THELISTENER_API UStationListenedCondition : public UEventGraphBaseConditionData
{
	GENERATED_BODY()

public:
	virtual struct FConditionKey GetKey() const override;
	static struct FConditionKey GenerateKey(const class UStationAsset* Station);

	virtual FText GetNodeTitle() const override {return FText::FromString("Station Listened");}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	class UStationAsset* Station;
};

UCLASS(DisplayName = "Has Finished Station", meta = (ToolTip = "Did this station finish"))
class THELISTENER_API UStationFinishedCondition : public UEventGraphBaseConditionData
{
	GENERATED_BODY()

public:
	virtual struct FConditionKey GetKey() const override;
	static struct FConditionKey GenerateKey(const class UStationAsset* Station);

	virtual FText GetNodeTitle() const override { return FText::FromString("Station Finished Condition"); }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	class UStationAsset* Station;
};

UCLASS(DisplayName = "Play Time Required",
	meta = (ToolTip = "Has enough time passed since the condition has been loaded ?"))
class THELISTENER_API UActiveTimeCondition : public UEventGraphBaseConditionData
{
	GENERATED_BODY()

public:
	virtual struct FConditionKey GetKey() const override;
	static struct FConditionKey GenerateKey(const FString& CName, float Time);

	virtual FText GetNodeTitle() const override {return FText::FromString("Timer");}

	void TimerFinish();

	virtual void SetupWorldContext(UObject* InWorldContextObject) override;

protected:
	void BeginDestroy() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	float TimeRequired;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FString ChronoName;

	FTimerHandle ActiveTimeHandle;
};

UCLASS(DisplayName = "Highlighted Selector",
	meta = (ToolTip = "Did the player highlight this selector in file"))
class THELISTENER_API USelectorHighlightCondition : public UEventGraphBaseConditionData
{
	GENERATED_BODY()

public:
	virtual struct FConditionKey GetKey() const override;
	static struct FConditionKey GenerateKey(UClass* InspectWidgetClass, FString Name);

	virtual FText GetNodeTitle() const override {return FText::FromString("Selector Highlighted");}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	TSubclassOf<class UInspectWidget> WidgetInspected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FString SelectorName;
};