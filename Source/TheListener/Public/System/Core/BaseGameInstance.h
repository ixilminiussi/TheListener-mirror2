#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BaseGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeControllerType);

enum EControllerType
{
	GENERIC,
	DUALSENSE
};

UCLASS()
class THELISTENER_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;
	void BindDualSenseEvents();

	// Input Prompts == Begin ==========================
public:
	int FindAssociatedInputMappingId(const FKey& Key) const;

	UFUNCTION(BlueprintCallable)
	class UTexture2D* GetCurrentControllerTypeTexture(int ID) const;

	UFUNCTION(BlueprintCallable)
	void ApplyPlaystationVisuals();
	UFUNCTION(BlueprintCallable)
	void ApplyGenericVisuals();
	
	UFUNCTION(BlueprintNativeEvent)
	void LoadRTPC(USettingsSave* Save);

private:
	UPROPERTY(EditDefaultsOnly)
	class UInputsTexturesDataAsset* InputTexturesDataAsset = nullptr;

	UPROPERTY()
	class UInputsTexturesDataAsset* InputTexturesCopy = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UInputsTexturesPlatformsDataAsset* InputsTexturesPlatformsDataAsset = nullptr;

	UPROPERTY()
	class UInputsTexturesPlatformsDataAsset* InputsTexturesPlatformsCopy = nullptr;

	EControllerType CurrentType = GENERIC;

	UPROPERTY()
	UUserWidget* CurrentLoadingScreenWidget = nullptr;

public:
	UPROPERTY(BlueprintAssignable, Category = "Luka|Events")
	FOnChangeControllerType OnChangeControllerType;
	// Input Prompts == End ============================

	UPROPERTY(EditDefaultsOnly)
	class USettingsDataAsset* SettingsDataAsset = nullptr;

	UFUNCTION()
	class USettingsSave* GetSettingsSave() const;

protected:
	UPROPERTY()
	TObjectPtr<class USettingsSave> SettingsSave = nullptr;
};
