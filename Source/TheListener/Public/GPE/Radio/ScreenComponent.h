#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScreenComponent.generated.h"


UCLASS()
class THELISTENER_API AScreenComponent : public AActor
{
	GENERATED_BODY()

public:
	AScreenComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetNiagaraFloatArray(const TArray<float>& Array);

	void SetRadio(class ARadio* Radio);

protected:
	void RefreshQNRT();
	
	UFUNCTION(BlueprintCallable)
	void GenerateNoiseArray();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Screen|QNRT")
	TObjectPtr<class UNiagaraComponent> NiagaraComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Screen")
	TObjectPtr<class UStaticMeshComponent> Plane;

	UPROPERTY(EditDefaultsOnly, Category = "Screen|QNRT")
	TObjectPtr<class UQNRTInfoDataAsset> QNRTInfoDataAsset;

	UPROPERTY()
	TObjectPtr<class ARadio> Parent;

	TArray<float> QNRTAnalysisArray{};

	UPROPERTY(BlueprintReadWrite, Category = "Screen|QNRT")
	TArray<float> BackgroundNoiseArray{};
	UPROPERTY(EditDefaultsOnly, Category = "Screen|QNRT")
	float BackgroundNoiseStrength = 0.6f;

	UPROPERTY(BlueprintReadWrite, Category = "Screen|QNRT")
	TArray<float> StaticNoiseArray{};
	UPROPERTY(EditDefaultsOnly, Category = "Screen|QNRT")
	float StaticStrength = 1.0f;
	
	FTimerHandle NoiseTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Screen|QNRT")
	float NoiseTimerInterval = 0.01f;
};
