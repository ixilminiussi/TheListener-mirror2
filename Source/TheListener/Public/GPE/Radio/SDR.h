#pragma once
#include "CoreMinimal.h"
#include "GPE/Toy.h"
#include "SDR.generated.h"

UCLASS()
class THELISTENER_API ASDR : public AToy
{
	GENERATED_BODY()

public:
	ASDR();

	UFUNCTION()
	void Refresh() const;

	UFUNCTION(BlueprintImplementableEvent)
	void SetBand(int Band);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void Clear() const;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|SDR")
	FColor ClearColor{0, 0, 0, 255};
	UPROPERTY(EditDefaultsOnly, Category = "Toy|SDR")
	float RefreshRate = 0.2f;
	
	FTimerHandle RefreshTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|SDR|Bands")
	class UTextureRenderTarget2D* LFRenderTarget;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|SDR|Bands")
	class UMaterialInstance* LFRenderer;
	UPROPERTY()
	class UMaterialInstanceDynamic* LFRendererDynamic;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|SDR|Bands")
	class UTextureRenderTarget2D* MFRenderTarget;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|SDR|Bands")
	class UMaterialInstance* MFRenderer;
	UPROPERTY()
	class UMaterialInstanceDynamic* MFRendererDynamic;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|SDR|Bands")
	class UTextureRenderTarget2D* HFRenderTarget;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|SDR|Bands")
	class UMaterialInstance* HFRenderer;
	UPROPERTY()
	class UMaterialInstanceDynamic* HFRendererDynamic;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Toy|SDR")
	class UStaticMeshComponent* BaseMeshComponent;
};
