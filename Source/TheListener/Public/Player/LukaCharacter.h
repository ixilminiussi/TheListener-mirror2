#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LukaCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMove, float, MoveSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLook);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartMoving);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopMoving);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginAnyInView);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndAnyInView);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndSpecificInView, AObji*, Obji);


UCLASS()
class THELISTENER_API ALukaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALukaCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MapPlayerData(const class UPlayerData*);
	UFUNCTION(BlueprintCallable, Category = "Luka")
	class UPlayerData * GetPlayerData() const;

	UFUNCTION()
	void MapSettingsData();

	friend class ALukaController;

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	UPROPERTY(EditDefaultsOnly, Category = "Luka", meta = (Tooltip = "Takes precedences over everything else"))
	TObjectPtr<class UPlayerData> PlayerData;
	UPROPERTY(EditDefaultsOnly, Category = "Luka")
	TObjectPtr<class USettingsDataAsset> SettingsData;
	// Basic Controls Begin
public:
	class UInputMappingContext* GetLukaMappingContext() const;

	UPROPERTY(BlueprintAssignable, Category = "Luka")
	FOnLook OnLook;
	UPROPERTY(BlueprintAssignable, Category = "Luka")
	FOnMove OnMove;

	UPROPERTY(BlueprintAssignable, Category = "Luka")
	FOnStartMoving OnStartMoving;
	UPROPERTY(BlueprintAssignable, Category = "Luka")
	FOnStopMoving OnStopMoving;

protected:
	void Look(const struct FInputActionValue& Value);
	void Move(const struct FInputActionValue& Value);

	UFUNCTION(BlueprintImplementableEvent, Category = "Luka")
	void OnStep(float Intensity);

	void HandleStartMoving(const FInputActionValue&Value);
	void HandleStopMoving(const FInputActionValue&Value);

	float MinFootstepDistance;
	float MaxFootstepDistance;
	float MaxFootstepTime;

	UPROPERTY(EditDefaultsOnly, Category = "Luka|Enhanced Input")
	class UInputAction* InputActionLook;
	UPROPERTY(EditDefaultsOnly, Category = "Luka|Enhanced Input")
	class UInputAction* InputActionMove;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Luka|Enhanced Input")
	class UInputMappingContext* InputMappingContext;

	float HorizontalSensitivity;
	float VerticalSensitivity;

	float LateralMaxWalkSpeed;
	float BackwardMaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Luka")
	TObjectPtr<class USpringArmComponent> SpringArmComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Luka", BlueprintReadOnly)
	TObjectPtr<class UCameraComponent> CameraComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Luka")
	TObjectPtr<class UHandComponent> HandComponent;

public:
	TObjectPtr<class UHandComponent> GetHandComponent() const;
	// Basic Controls End

	// Interactives Begin
	float GetInteractiveMaxRange() const { return InteractionMaxRange; }
	bool HasObji() const;
	class AObji* GetHeldObji() const;

	UPROPERTY(BlueprintAssignable, Category = "Luka|Events")
	FOnBeginAnyInView OnBeginAnyInView;
	UPROPERTY(BlueprintAssignable, Category = "Luka|Events")
	FOnEndAnyInView OnEndAnyInView;
	UPROPERTY(BlueprintAssignable, Category = "Luka|Events")
	FOnEndSpecificInView OnEndSpecificInView;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Luka|Interaction")
	float InteractionMaxRange{50.f};

	UPROPERTY(EditDefaultsOnly, Category = "Luka|Interaction")
	TEnumAsByte<ECollisionChannel> InteractiveCollisionChannel;

	void InteractiveRayCheck();
	// Toys Begin
public:
	class AToy* GetToyInView() const;

protected:
	UPROPERTY()
	TObjectPtr<class AToy> ToyInView;
	// Toys End

	// Interactables Begin

	void Interact();

public:
	UFUNCTION(BlueprintCallable, Category = "Luka|Obji")
	void PickupObji(class AObji* Obji);
	UFUNCTION(BlueprintCallable, Category = "Luka|Obji")
	AObji* DropObji();
	class AInteractable* GetInteractableInView() const;

protected:
	UPROPERTY()
	class AInteractable* InteractableInView;
	// Interactables End

	// Snap to Ground Begin
public:
	void SnapToGround(float DistanceCheck = 1000.f);
	// CR LookAtToy ?= Snap To Ground ?
	void LookAtToy(const class AToy* Toy);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Luka|World")
	TEnumAsByte<ECollisionChannel> FloorCollisionChannel;
	// Snap to Ground End
};
