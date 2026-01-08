#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CommandWidget.generated.h"

UCLASS()
class THELISTENER_API UCommandWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeOnInitialized() override;
	
	void CreateImages() const;
	void SetName(const FString& Name);
	void SetInputAction(class UInputAction* InputAction);
	void ClearImages();
	void UpdateImages(const TArray<struct FEnhancedActionKeyMapping> Mapping);

protected:
	UPROPERTY()
	FString NameText {};

	UPROPERTY()
	class UInputAction* AssociatedInputAction = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* ImagesHorizontalBox = nullptr;

	UPROPERTY()
	TArray<class UCommonLazyImage*> InputImages {};
	
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* InputText = nullptr;
	
	int ImagesLimit = 4;
	
	UFUNCTION()
	void ModifyControllerTextures();

	UPROPERTY()
	TArray<int> IDs{};
};
