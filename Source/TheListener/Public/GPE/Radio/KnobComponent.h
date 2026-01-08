#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "KnobComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClick, bool, IsIncreasing);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THELISTENER_API UKnobComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UKnobComponent(const FObjectInitializer& ObjectInitializer);

	/**
	 * 
	 * @param Amount Pick a value between -1 and 1;
	 */
	void ClockwiseRotate(float Amount, float DeltaTime);

	void SetRange(const FVector2D& NewRange);
	UFUNCTION(BlueprintCallable)
	void SetValue(float NewValue);
	UFUNCTION(BlueprintCallable)
	float GetValue() const;

protected:
	virtual void BeginPlay() override;
	void ApplyClamp();

	UPROPERTY(EditAnywhere, Category = "Knob", meta = (ClampMin = "0."))
	float ValueSensitivity;
	UPROPERTY(EditAnywhere, Category = "Knob", meta = (ClampMin = "0."))
	float RotationSensitivity;
	UPROPERTY(EditAnywhere, Category = "Knob")
	TObjectPtr<UCurveFloat> SensitivityCurve;

	UPROPERTY(EditAnywhere, Category = "Knob")
	bool Flip;

	float Value;
	FVector2D Range;

	// Click Begin ===========================
	void ClickTest() const;

	UPROPERTY(EditAnywhere, Category = "Knob", meta = (ClampMin = "0.", ClampMax = "90."))
	float ClickAngle;

	UPROPERTY(BlueprintReadOnly, Category = "Knob")
	class UAkComponent* ClickAkComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Knob")
	class UAkAudioEvent* StartClickAudioEvent;

	UPROPERTY(BlueprintAssignable, Category = "Knob")
	FOnClick OnClick;
	// Click End =============================
};
