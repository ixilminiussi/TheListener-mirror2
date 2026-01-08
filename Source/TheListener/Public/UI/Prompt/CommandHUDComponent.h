#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CommandHUDComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THELISTENER_API UCommandHUDComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCommandHUDComponent();

protected:
	virtual void BeginPlay() override;

public:
	void Generate(class UInputMappingContext* InputMappingContext = nullptr);

protected:
	void SpawnCommandHelpers(APlayerController*);

	UPROPERTY()
	class UInputMappingContext* GlobalMappingContext = nullptr;
	UPROPERTY()
	class UInputMappingContext* LukaMappingContext = nullptr;

	UPROPERTY()
	class ALukaHUD* LukaHUD = nullptr;


	/// Hover Part
public:
	void AddHoverToHUD();
	void RemoveHoverFromHUD();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI|Hover")
	TSubclassOf<class UHoverCommandWidget> HoverCommandWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="UI|Hover")
	TMap<FString, class UInputAction*> HoverCommands{};

	UPROPERTY()
	TArray<class UHoverCommandWidget*> HoverCommandWidgets{};


	/// Active Part
public:
	void AddActiveToHUD();
	void RemoveActiveFromHUD();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI|Active")
	TSubclassOf<class UActiveCommandWidget> ActiveCommandWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="UI|Active")
	TMap<FString, class UInputAction*> ActiveCommands{};

	UPROPERTY()
	TArray<class UActiveCommandWidget*> ActiveCommandWidgets{};

	UPROPERTY()
	class UInputMappingContext* ExternalMappingContext = nullptr;
};
