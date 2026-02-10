// Copyright (c) 2025 ArtFX. Created by Team FalseStart. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toy.h"
#include "Components/TimelineComponent.h"
#include "CorkboardDiegetic.generated.h"

UCLASS()
class THELISTENER_API ACorkboardDiegetic : public AToy
{
	GENERATED_BODY()

	ACorkboardDiegetic();
	
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnPossessAfterTransition_Implementation() override;
	virtual void OnUnPossessAfterTransition_Implementation() override;

	FVector FindClosestPositionOnPlane(FVector Point);
	FVector2D FindPositionOnPlaneByRatio(FVector Point);
	

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|Mesh")
	class UStaticMeshComponent* BoardMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|Mesh")
	class UStaticMeshComponent* CameraPlaneMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|Enhanced Input")
	class UInputAction* InputActionZoom = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|Enhanced Input")
	class UInputAction* InputActionMove = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|Translation")
	float CurrentZoomScale = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|Translation")
	float ZoomScaleSpeed = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|Translation")
	TObjectPtr<UCurveFloat> FOVCurve;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|Translation")
	TObjectPtr<UCurveFloat> CamSpeedCurve;

	FVector2D LastMoveInputValue;
	FVector LocationDL ;
	FVector LocationUL ;
	FVector LocationDR ;
	FVector LocationUR ;
	FVector PlaneNormal;
	FVector PlaneAxisX;
	FVector PlaneAxisY;
	float PlaneWidth;
	float PlaneHeight;
	float PlaneDistance;
	
	FVector2D CurrentPosition = FVector2D(0.5f,0.5f);

	TMap<FString, TWeakObjectPtr<class ACorkboardNoteActor>> ClueActorsMap;
	TMap<class ACorkboardNoteActor*,TArray<TWeakObjectPtr<class ACorkboardLinkActor>>> ClueLinkedMap;

	float LastZoomInputValue;

	TQueue<FString> CluesUpdated;
	FString CurrentDiscoveringClue;
	FVector DiscoverStartLocation;
	FVector DiscoverEndLocation;

	FOnTimelineFloat DiscoverInterpFunction{};
	FOnTimelineEvent DiscoverFinishedFunction{};

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTimelineComponent* DiscoverTimelineComponent;
	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* DiscoverTimelineCurve;

public:
	auto UpdateClue(FString ClueString, bool bActivate) -> void;

	void MoveInput(const struct FInputActionValue& Value);
	void ZoomInput(const struct FInputActionValue& Value);

	TArray<FString> GetClues();

	UFUNCTION()
	void StartDiscoverTimeline();

	UFUNCTION()
	void ProgressDiscoverTimeline(float value);
	
	UFUNCTION()
	void FinishDiscoverTimeline();
	
};
