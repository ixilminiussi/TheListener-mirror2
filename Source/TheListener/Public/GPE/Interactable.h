#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS()
class THELISTENER_API AInteractable : public AActor
{
	GENERATED_BODY()

public:
	AInteractable(const FObjectInitializer& ObjectInitializer);
	virtual void Interact(const class ALukaCharacter* Luka);

	bool IsEnabled() const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Enable(bool bToggle);

protected:
	bool bEnabled{true};

	virtual void BeginPlay() override;
	virtual void OnInteract(class AController* NewController);
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnInteract_();

	UPROPERTY()
	class ALukaController* LukaController;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interaction|Enhanced Input")
	class UInputMappingContext* InputMappingContext;


	// Components Begin ==================================
public:
	class UStaticMeshComponent* GetStaticMeshComponent() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	class UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	class UBoxComponent* CollisionComponent;
	// Components End ====================================


	// UI Begin ==========================================
public:
	class UInputMappingContext* GetInputMappingContext() const;
	void SetHoverWidgetVisibility(const bool bIsVisible) const;
	void SetActiveWidgetVisibility(const bool bIsVisible) const;

protected:
	UPROPERTY(EditDefaultsOnly)
	class UCommandHUDComponent* CommandHUDComponent = nullptr;
	// UI End =============================================
};
