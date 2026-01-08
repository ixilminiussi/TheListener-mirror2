#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "Obji.generated.h"

enum class EInteractionsTypes : uint8;

struct FDropZoneInfo
{
	FVector Location;
	FRotator Rotation;

	bool IsValid() const
	{
		return bValid;
	}

	void Fail()
	{
		bValid = false;
	}

	void Succeed()
	{
		bValid = true;
	}

protected:
	bool bValid{false};
};

UCLASS()
class THELISTENER_API AObji : public AInteractable
{
	GENERATED_BODY()

public:
	AObji(const FObjectInitializer& ObjectInitializer);

	void CheckAcceptableDropZone(FDropZoneInfo& DropZoneInfo, const FVector& RayCheckBegin,
	                             const FVector& RayCheckEnd, FCollisionQueryParams QueryParams = {});
	AObji* CreatePreview() const;
	bool CheckValidity(const FVector& Location, const FQuat& QuatRotation) const;

	TArray<TSubclassOf<AInteractable>> GetInteractablesAllowed() const;
	TArray<TSubclassOf<class AToy>> GetToysAllowed() const;

	virtual void Interact(const class ALukaCharacter* Luka) override;
	[[nodiscard]] virtual bool Drop();

protected:
	virtual void CustomCheck(FDropZoneInfo& DropZoneInfo, const FVector& RayCheckBegin,
	                         const FVector& RayCheckEnd, FCollisionQueryParams QueryParams = {});

	virtual void OnInteract(class AController* NewController) override;
	virtual void OnDrop();

	UFUNCTION(BlueprintImplementableEvent, Category = "Obji")
	void OnPickup_();

	UFUNCTION(BlueprintImplementableEvent, Category = "Obji")
	void OnDrop_();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obji")
	class UMaterialInstance* PreviewMaterial;

	// Inputs Start =======================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Obji|Enhanced Input")
	class UInputAction* InputActionPreviewRotation;

	virtual void SetupInput(UEnhancedInputComponent* EnhancedInputComponent);
	// Inputs End =========================================

	FDropZoneInfo LastDropZoneInfo{};

	// Rotation Begin ====================================
public:
	virtual void RotatePreview(const struct FInputActionValue& Value);

protected:
	UPROPERTY(EditAnywhere, Category = "Obji|Interaction", meta = (ClampMin = ".5", ClampMax = "90."))
	float RotationStrength;

	float AdditiveRotation{0.f};
	// Rotation End ======================================

	// Transitions Begin =================================
public:
	struct FTimeline* MoveTo(const class UHandComponent* TargetComponent, bool bTeleport = false);
	struct FTimeline* MoveTo(const FVector& Location, bool bTeleport = false);
	struct FTimeline* RotateTo(const class UHandComponent* Rotation, bool bTeleport = false);
	struct FTimeline* RotateTo(const FRotator& Rotation, bool bTeleport = false);

	void TickMoveBlendTimeline();
	void TickRotateBlendTimeline();

	bool IsMoving() const { return bMoving; }
	bool IsRotating() const { return bRotating; }

protected:
	UPROPERTY()
	const class UHandComponent* TargetLocationComponent;
	FVector TargetLocation;
	FVector BeginLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Obji|Interaction")
	class UCurveFloat* MoveBlendCurve;
	UPROPERTY(EditDefaultsOnly, Category = "Obji|Interaction")
	TArray<TSubclassOf<AInteractable>> AllowedInteractables;
	UPROPERTY(EditDefaultsOnly, Category = "Obji|Interaction")
	TArray<TSubclassOf<AToy>> AllowedToys;
	struct FTimeline* MoveBlendTimeline;
	FTimerHandle MoveBlendTimer;

	UFUNCTION()
	void EffectMoveBlend(float Value);

	UFUNCTION()
	void EffectRotateBlend(float Value);

	void CreateTimelines();

	UPROPERTY()
	const class UHandComponent* TargetRotationComponent;
	FRotator TargetRotation;
	FRotator BeginRotation;

	UPROPERTY(EditDefaultsOnly, Category = "Obji|Interaction")
	class UCurveFloat* RotateBlendCurve;
	struct FTimeline* RotateBlendTimeline;
	FTimerHandle RotateBlendTimer;

	bool bMoving{false};
	bool bRotating{false};
	// Transitions End ===================================
};
