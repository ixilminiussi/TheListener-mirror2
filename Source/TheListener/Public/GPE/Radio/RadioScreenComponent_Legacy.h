#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RadioScreenComponent_Legacy.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THELISTENER_API URadioScreenComponent_Legacy : public USceneComponent
{
	GENERATED_BODY()

public:
	URadioScreenComponent_Legacy(const FObjectInitializer& ObjectInitializer);
	void SetupAttachments(USceneComponent* SceneComponent) const;

	void SetReceiverFrequency(float Frequency);
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	float ReceiverFrequency;

	FVector2D GlobalRange;
	UPROPERTY(EditDefaultsOnly, Category = "RadioScreen", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
	float LocalRange{500.f};

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* SpectrogramStaticMesh;
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* ScreenStaticMesh;

	UPROPERTY(EditDefaultsOnly, Category = "RadioScreen")
	class UMaterialInterface* SpectrogramMaterialInterface;
	UPROPERTY()
	class UMaterialInstanceDynamic* SpectrogramMaterialDynamic;
};