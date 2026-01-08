// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CorkboardWidget.generated.h"

USTRUCT()
struct FNoteLink
{
	GENERATED_BODY()
	class UCorkboardNoteWidget* CLueWidgetA;
	class UCorkboardNoteWidget* CLueWidgetB;

	class UImage* Image;

	bool operator==(TTuple<class UCorkboardNoteWidget*, class UCorkboardNoteWidget*>) const;

	void Init(class UCorkboardNoteWidget* CWA, class UCorkboardNoteWidget* CWB,
	          TObjectPtr<class UWidgetTree> WidgetTree, class UCanvasPanel* ContentPanel, float LineThickness,
	          UTexture2D* LineTexture);
};

/**
 * 
 */
UCLASS()
class THELISTENER_API UCorkboardWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetMoveSpeed(float InSpeed);
	void SetZoomSpeed(float InZoomSpeed);
	void SetZoomLimits(FVector2D InZoomLimits);
	void MoveInput(const struct FInputActionValue& Value);
	void ZoomInput(const struct FInputActionValue& Value);

	void AddLink(UCorkboardNoteWidget* LinkA, UCorkboardNoteWidget* LinkB);

	void Reset();

	void UpdateClue(class UClueAsset* clue);

	void SetupNoteWidgets();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|UI", meta = (BindWidget))
	class UCanvasPanel* ContentCanvasPanel;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|UI", meta = (BindWidget))
	class UCanvasPanel* BaseCanvasPanel;


	class UCanvasPanelSlot* ContentCanvasPanelSlot;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|UI")
	TMap<class UClueAsset*, class UCorkboardNoteWidget*> NoteWidgets;
	TArray<FNoteLink> NoteLinks;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|UI|Links")
	UTexture2D* LinkTexture;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|UI|Links")
	float LinkThickness;

	FVector2D CurrentAlignment;
	float MoveSpeed = 0.5f;
	FVector2D MoveDirection;

	FVector2D BaseSize;

	float CurrentZoom = 1;
	FVector2D TrueZoom = FVector2D::Zero();
	float ZoomSpeed = 0.5f;
	float ZoomLastInput = 0;
	FVector2D ZoomLimits;

	void Move(float delta);
	void Zoom(float delta);
};
