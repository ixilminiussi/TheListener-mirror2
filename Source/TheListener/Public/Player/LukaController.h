#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LukaController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPossessToy);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPossessToyTransition);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnpossessToy);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnpossessToyTransition);


UCLASS()
class THELISTENER_API ALukaController : public APlayerController
{
	GENERATED_BODY()

public:
	ALukaController();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void SetPawn(APawn* InPawn) override;

	class UInputMappingContext* GetGlobalMappingContext() const;
	void SetGlobalInputsStatus(bool IsRunning) const;

	void MapPlayerData(const class UPlayerData*);

	UFUNCTION(BlueprintCallable)
	class ALukaCharacter* GetLukaCharacter() const;


	// CR No references found using these events (C++ & BP)?
	UPROPERTY(BlueprintAssignable, Category = "Luka|Events")
	FOnPossessToyTransition OnPossessToyTransition;
	UPROPERTY(BlueprintAssignable, Category = "Luka|Events")
	FOnUnpossessToyTransition OnUnpossessToyTransition;
	UPROPERTY(BlueprintAssignable, Category = "Luka|Events")
	FOnPossessToy OnPossessToy;
	UPROPERTY(BlueprintAssignable, Category = "Luka|Events")
	FOnUnpossessToy OnUnpossessToy;

	UFUNCTION(BlueprintCallable)
	void KickFromCurrentToy();

	UPROPERTY(EditDefaultsOnly, Category = "Luka")
	class UInspectingComponent* InspectingComponent;

protected:
	UPROPERTY()
	TObjectPtr<class ALukaCharacter> Luka;
	UPROPERTY()
	TObjectPtr<class AToy> PossessedToy;
	UPROPERTY()
	TObjectPtr<class AFile> ActiveFileReading = nullptr;
	UPROPERTY()
	TObjectPtr<class ALukaHUD> LukaHUD = nullptr;

	void Interact(const struct FInputActionValue& Value);
	void Return(const struct FInputActionValue& Value);
	void Drop(const struct FInputActionValue& Value);

	void MovePlayerBehindToy(const class AToy* Toy) const;

	void PauseGame(const struct FInputActionValue& Value);

	void PossessToy(class AToy* Toy);
	void EnactPossession();
	void ReleaseToy(class AToy* Toy);

	bool bTransitionMutex{false};

	// Inputs
	UPROPERTY(EditDefaultsOnly, Category = "Luka|Enhanced Input")
	TObjectPtr<class UInputMappingContext> GlobalInputMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = "Luka|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionInteract;
	UPROPERTY(EditDefaultsOnly, Category = "Luka|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionReturn;
	UPROPERTY(EditDefaultsOnly, Category = "Luka|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionDrop;
	UPROPERTY(EditDefaultsOnly, Category = "Luka|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionPause;

	// Camera Transitions
	float CameraBlendTime;
	float TransitionExponential;
	FTimerHandle CameraDelayHandle;
};
