#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LukaCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMove, float, MoveSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLook);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartMoving);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopMoving);

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
	// Basic Controls End

public:
	class USceneComponent* GetDrop() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Luka|Interaction")
	TObjectPtr<class UPickupComponent> PickupComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Luka|Interaction")
	TObjectPtr<class UInteractComponent> InteractComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Luka|Interaction")
	TObjectPtr<class USceneComponent> DropComponent;
	
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
	TObjectPtr<class AActor> ActorInView;
	// Toys End
	void TryShowHoverPrompt(AActor *Actor);
	void TryHideHoverPrompt(AActor *Actor);

	void Interact() const;
	void Return() const;
	void Drop() const;

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
