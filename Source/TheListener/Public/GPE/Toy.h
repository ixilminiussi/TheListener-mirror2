#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Toy.generated.h"

UCLASS()
class THELISTENER_API AToy : public APawn
{
	GENERATED_BODY()

public:
	AToy();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Toy")
	const class UCameraComponent* GetCameraComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Toy|Interactive")
	void Lock(bool bEnable);
	bool IsLocked() const;
	bool IsEnabled() const;

	UFUNCTION(BlueprintCallable, Category = "Toy|Interactive")
	void Enable(bool bToggle);
	void Highlight(bool bToggle);

	UFUNCTION(BlueprintCallable, Category = "Toy|Interactive")
	void KickPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void Tick(float DeltaTime) override;

	void Zoom(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	FRotator TargetRotation;
	FRotator StartingRotation;
	float TargetZoom;
	float StartingZoom;
	bool bLooking;
	bool bZooming;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Camera")
	float ZoomLerpStrength = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Camera")
	float ZoomAmount = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Camera")
	float LookLerpStrength = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category="Toy|Camera")
	FVector2D MaxRotation;

	UPROPERTY(EditDefaultsOnly, Category="Toy|Interactive")
	TObjectPtr<class UMaterial> HighlightMaterial;

	UPROPERTY(EditInstanceOnly, Category="Toy", BlueprintReadWrite)
	TObjectPtr<class UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category="Toy|Interactive")
	TObjectPtr<class UBoxComponent> CollisionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Toy|Enhanced Input")
	TObjectPtr<class UInputMappingContext> InputMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Toy|Enhanced Input")
	TObjectPtr<class UInputMappingContext> FunMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionCameraLook;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionCameraZoom;

	bool bLocked{false};

	UPROPERTY(EditAnywhere, Category = "Toy")
	bool bEnabled{true};

	bool bIsPossessed{false};

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Interactive")
	float CameraDistanceBack{20.0f};

public:
	float GetCameraDistanceBack() const;

	// Feedbacks ==============================
	void OnPossessToyTransition();
	virtual void OnUnpossessToyTransition();
	void ShowActivePrompts(const bool bIsVisible) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Toy|Feedbacks")
	void OnPossessBeforeTransition();
	UFUNCTION(BlueprintNativeEvent, Category = "Toy|Feedbacks")
	void OnPossessAfterTransition();
	UFUNCTION(BlueprintNativeEvent, Category = "Toy|Feedbacks")
	void OnUnPossessBeforeTransition();
	UFUNCTION(BlueprintNativeEvent, Category = "Toy|Feedbacks")
	void OnUnPossessAfterTransition();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Interactive")
	TArray<FName> OnActivePrompts;

	UPROPERTY()
	TObjectPtr<class UAkComponent> AkComponent{nullptr};
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Feedbacks")
	TObjectPtr<class UAkAudioEvent> OnPossessTransitionAkEvent{nullptr};
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Feedbacks")
	TObjectPtr<class UAkAudioEvent> OnUnpossessTransitionAkEvent{nullptr};
	// Feedbacks End ==============================
};
