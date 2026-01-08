// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GPE/Obji.h"
#include "Player/Components/Inspectable.h"
#include "File.generated.h"

UENUM()
enum class ELukaTextVisibility : uint8
{
	ELTV_Invisible UMETA(DisplayName = "Invisible"),
	ELTV_Visible UMETA(DisplayName = "Visible")
};

UCLASS()
class THELISTENER_API AFile : public AObji, public IInspectable

{
	GENERATED_BODY()

public:
	explicit AFile(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	// Interface IInspectable
	virtual class UInspectWidget* GetInspectWidget() override;
	virtual void ShowInspectable() override;
	virtual void HideInspectable() override;
	virtual bool IsInspectableVisible() override;

	//File 
protected:
	virtual void CustomCheck(FDropZoneInfo& DropZoneInfo, const FVector& RayCheckBegin,
	                         const FVector& RayCheckEnd, FCollisionQueryParams QueryParams = {}) override;

	virtual void OnDrop() override;

	//Widget
	UPROPERTY(EditAnywhere, Category = "Obji|File|Inspectable")
	TSubclassOf<class UInspectWidget> InspectWidgetClass = nullptr;

	class UInspectWidget* InspectWidget;

	/// Feedbacks
	UPROPERTY(EditDefaultsOnly, Category = "Obji|File|Audio")
	class USceneComponent* SoundPosition = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Obji|File|Audio")
	class UAkComponent* AkComponent = nullptr;
};
