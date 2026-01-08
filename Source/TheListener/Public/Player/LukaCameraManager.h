#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "LegacyCameraShake.h"
#include "LukaCameraManager.generated.h"

UCLASS()
class THELISTENER_API ALukaCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ALukaCameraManager();
	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPossessToy();
	UFUNCTION()
	void OnUnpossessToy();
	UFUNCTION()
	void OnMove(float Speed);
	UFUNCTION()
	void OnStopMoving();

	void SetTargetShakeAsIdleShake();
	
protected:
	UPROPERTY()
	TObjectPtr<ULegacyCameraShake> CameraShake{nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Luka|Camera")
	TSubclassOf<ULegacyCameraShake> CameraShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Luka|Camera")
	struct FROscillator IdleShake;
	UPROPERTY(EditDefaultsOnly, Category = "Luka|Camera")
	struct FROscillator MaxWalkShake;
	UPROPERTY(EditDefaultsOnly, Category = "Luka|Camera")
	struct FROscillator ToyShake;

	UPROPERTY()
	struct FROscillator TargetShake;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"), Category = "Luka|Camera")
	float LerpAlpha = .0f;
};
