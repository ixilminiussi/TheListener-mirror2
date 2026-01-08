#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HandComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THELISTENER_API UHandComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UHandComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	FVector GetTrueObjiLocation() const;
	void UpdateObjiPosition() const;

	bool HasObji() const { return HeldObji != nullptr; }
	class AObji* GetHeldObji() const;

	void PickupObji(class AObji* Obji);
	AObji* ReleaseObji();

	void PrepareTraceParams(struct FCollisionQueryParams& QueryParams) const;

protected:
	virtual void BeginPlay() override;
	void UpdatePreviewLocation();

	UPROPERTY()
	const class UCameraComponent* HeadCamera;
	UPROPERTY()
	class AObji* ObjiPreview;
	bool bObjiPreviewIsValid{false};
	UPROPERTY()
	class AObji* HeldObji;

	UPROPERTY()
	const class ALukaCharacter* LukaParent;

	UPROPERTY(EditDefaultsOnly, Category = "Hand|Anchors", meta = (ClampMin = "0", ClampMax = "1000"))
	float TopAnchor{TNumericLimits<float>::Max()};
	UPROPERTY(EditDefaultsOnly, Category = "Hand|Anchors", meta = (ClampMin = "0", ClampMax = "1000"))
	float LeftAnchor{TNumericLimits<float>::Max()};
	UPROPERTY(EditDefaultsOnly, Category = "Hand|Anchors", meta = (ClampMin = "0", ClampMax = "1000"))
	float RightAnchor{TNumericLimits<float>::Max()};
	UPROPERTY(EditDefaultsOnly, Category = "Hand|Anchors", meta = (ClampMin = "0", ClampMax = "1000"))
	float BottomAnchor{TNumericLimits<float>::Max()};
};