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
	virtual bool Interact(AActor *Key);

	bool IsEnabled() const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Enable(bool bToggle);

	bool TestKey(AActor *Key);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TArray<AActor*> Keys;
protected:
	bool bEnabled{true};

	virtual void BeginPlay() override;
	virtual void OnInteract();
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
	// Components End ====================================


	// UI Begin ==========================================
public:
	class UInputMappingContext* GetInputMappingContext() const;
	// UI End =============================================
};
