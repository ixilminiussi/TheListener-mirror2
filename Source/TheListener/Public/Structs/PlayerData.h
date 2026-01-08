#pragma once

#include "CoreMinimal.h"
#include "PlayerData.generated.h"

UCLASS(BlueprintType)
class THELISTENER_API UPlayerData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float FOV = 75.f;
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float CamHeight = 90.f;
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float Overscan = .3f;
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float ClampUp;
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float ClampDown;
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float HorizontalSensitivity = 1.2f;
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float VerticalSensitivity = 0.7f;
	UPROPERTY(EditDefaultsOnly, Category = "Camera | Feedbacks")
	float CameraBlendTime = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Camera | Feedbacks")
	float TransitionExponential = 3.f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Movement")
	float MaxWalkSpeed = 250.f;
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float LateralMaxWalkSpeed = 200.f;
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float BackwardMaxWalkSpeed = 150.f;
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float MaxAcceleration = 600.f;
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float BrakingDecelerationWalking = 100.f;
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float MaxStepHeight;
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float GroundFriction = 8.f;
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float WalkableFloorAngle = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement | Feedbacks")
	float MinFootstepDistance;
	UPROPERTY(EditDefaultsOnly, Category = "Movement | Feedbacks")
	float MaxFootstepDistance;
	UPROPERTY(EditDefaultsOnly, Category = "Movement | Feedbacks")
	float MaxFootstepTime;

	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	float InteractionRange = 250.f;
};
