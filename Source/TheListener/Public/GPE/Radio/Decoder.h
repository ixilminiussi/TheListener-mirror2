#pragma once

#include "CoreMinimal.h"
#include "StationDataAsset.h"
#include "GameFramework/Actor.h"
#include "Decoder.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToggle, bool, bToggle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinish);

UCLASS()
class THELISTENER_API ADecoder : public AActor
{
	GENERATED_BODY()

public:
	ADecoder();

protected:
	virtual void BeginPlay() override;

	bool bEnabled = true;

	friend class FCogGameplayWindow_Utilities;

public:
	void SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInputComponent);
	UFUNCTION()
	void OnEndToyPossessEvent();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	bool StartDecoder(class ALukaController* Controller);
	bool StartDecoder(const class ALukaController* Controller, float InTargetLeft, float InTargetRight, bool bComplex);
	void StopDecoder();

	UFUNCTION(BlueprintCallable, Category = "Decoder")
	void Toggle(bool bToggle);

	UPROPERTY(BlueprintAssignable, Category = "Decoder")
	FOnToggle OnToggle;

	UPROPERTY(BlueprintAssignable, Category = "Decoder")
	FOnFinish OnFinish;
	
	// Inputs =================================
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Decoder|Enhanced Input")
	TObjectPtr<class UInputMappingContext> DecoderInputMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = "Decoder|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionSpinLeftKnob{nullptr};
	UPROPERTY(EditDefaultsOnly, Category = "Decoder|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionSpinRightKnob{nullptr};
	void RemoveMappingContext() const;

	// Left Joystick
	UFUNCTION()
	void SpinLeftKnob(const struct FInputActionValue& Value);
	bool bIsNotFirstLeftRollAngle = false;
	float LastLeftAngle = .0f;

	// Right Joystick
	UFUNCTION()
	void SpinRightKnob(const struct FInputActionValue& Value);
	bool bIsNotFirstRightRollAngle = false;
	float LastRightAngle = .0f;

	UPROPERTY(EditDefaultsOnly, Category = "Decoder|Controls|Joystick", meta = (ClampMin = "0.01", ClampMax = "1.0"))
	float MinimumRollInputRegister = .6f;
	// Inputs End =============================

	// Left/Right Knob ========================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decoder")
	TObjectPtr<class UKnobComponent> LeftKnobComponent{nullptr};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decoder")
	TObjectPtr<class UKnobComponent> RightKnobComponent{nullptr};
	UPROPERTY(EditDefaultsOnly, Category = "Decoder", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	FVector2D ValuesRange{};

public:
	FVector2d GetFrequencyRange() const;
	// Left/Right Knob End ====================

	// Objective ==============================
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Decoder", meta = (ClampMin = "0.0", ClampMax = "20.0"))
	float TargetsThreshold = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Decoder", meta = (ClampMin = "0.0", ClampMax = "5.0"))
	float ValidationDelay = 1.f;
	FTimerHandle DelayHandle;

	float TargetLeft, TargetRight;
	bool IsLeftDone = false;
	bool IsRightDone = false;
	bool PendingLaunching = false;

	bool CheckTarget(float TargetFrequency, float CurrentValue) const;
	void CheckAll(bool isLeftDone);
	UFUNCTION()
	void CompleteObjective();
	// Objective End ========================== 

	// Feedbacks ==============================
	UFUNCTION(BlueprintImplementableEvent, Category = "Decoder")
	void OnLeftToggle(bool bToggle);
	UFUNCTION(BlueprintImplementableEvent, Category = "Decoder")
	void OnRightToggle(bool bToggle);

	void ResetMaterial() const;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Decoder")
	TObjectPtr<class UStaticMeshComponent> BodyMesh{nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Decoder|Feedbacks")
	TObjectPtr<class UMaterialInstance> RunningMaterial{nullptr};
	UPROPERTY(EditDefaultsOnly, Category = "Decoder|Feedbacks", meta = (ClampMin = "1.0", ClampMax = "50.0"))
	float Ratio = 40.f;
	UPROPERTY();
	TObjectPtr<class UMaterialInstanceDynamic> RunningMaterialInstance{nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Decoder|Feedbacks")
	TObjectPtr<class UMaterial> CompletedMaterial{nullptr};

	void SetActiveWidgetVisibility(bool bIsVisible) const;
	UPROPERTY(EditDefaultsOnly, Category = "Decoder|Feedbacks")
	TObjectPtr<class UCommandHUDComponent> CommandHUDComponent = nullptr;
	// Feedbacks End ========================== 
};
