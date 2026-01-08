// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CorkboardNoteWidget.generated.h"

/**
 * 
 */
enum class EClueState : uint8;

UCLASS()
class THELISTENER_API UCorkboardNoteWidget : public UCommonUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|UI", meta = (BindWidget))
	class URichTextBlock* ClueTitleTextBox;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|UI", meta = (BindWidget))
	class URichTextBlock* ClueShortDescTextBox;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|UI", meta = (BindWidget))
	class UCanvasPanel* NoteCanvas;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|UI", meta = (BindWidget))
	class UImage* ClueImage;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|UI", meta = (BindWidget))
	class UImage* PinImage;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|UI", meta = (BindWidget))
	class UImage* StateImage;

	void SetTitleText(FString text);
	void SetShortDescriptionText(FString text);
	void SetClueImage(UTexture2D* tex);

	UFUNCTION(BlueprintCallable)
	void LoadCLue();

	virtual void NativeConstruct() override;

public:
	FVector2D GetPinPosition() const;

	UImage* GetPin() const;
	UImage* GetFakePin();
	void SetFakePin(UImage* Pin);

	UPROPERTY(EditAnywhere, Category = "Toy|Corkboard|Clue")
	TObjectPtr<class UClueAsset> Clue;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|State")
	UTexture2D* PartialStateImage;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Corkboard|State")
	UTexture2D* DisabledStateImage;

	UImage* FakePinImage;

	void UpdateStateImage(EClueState state);
};
