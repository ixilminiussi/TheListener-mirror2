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

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void Clear() const;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|SDR")
	FColor ClearColor{0, 0, 0, 255};
	UPROPERTY(EditDefaultsOnly, Category = "Toy|SDR")
	float RefreshRate = 0.2f;
	
	FTimerHandle RefreshTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|SDR")
	class UTextureRenderTarget2D* RenderTarget;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|SDR")
	class UMaterial* ScreenMaterial;
	UPROPERTY()
	class UMaterialInstanceDynamic* ScreenMaterialDynamic;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Toy|SDR")
	class UStaticMeshComponent* BaseMeshComponent;
};
