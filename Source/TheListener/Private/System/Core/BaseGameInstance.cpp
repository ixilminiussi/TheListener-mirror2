#include "System/Core/BaseGameInstance.h"

#include "SonyGamepadProxy.h"
#include "WindowsDualsense_ds5w.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Input/InputsTexturesDataAsset.h"
#include "UI/Input/InputsTexturesPlatformsDataAsset.h"
#include "UI/Menus/SettingsSave.h"
#include "Blueprint/UserWidget.h"

void UBaseGameInstance::Init()
{
	Super::Init();
	InputTexturesCopy = DuplicateObject<UInputsTexturesDataAsset>(InputTexturesDataAsset, this);
	InputsTexturesPlatformsCopy = DuplicateObject<UInputsTexturesPlatformsDataAsset>(
		InputsTexturesPlatformsDataAsset, this);

	//FCoreUObjectDelegates::PreLoadMap.AddUObject(this,&UBaseGameInstance::PreLoadingScreen);
	//FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this,&UBaseGameInstance::PostLoadingScreen);
	
	if (auto* DualSenseModule = &FWindowsDualsense_ds5wModule::Get())
	{
		// The device manager can be not initialised yet, the DeviceManagerReady event will trigger when the device manager is available
		if (auto DeviceManager = DualSenseModule->GetDeviceManager())
		{
			DeviceManager->OnDualSenseDisconnected.AddUObject(this, &UBaseGameInstance::ApplyGenericVisuals);
			DeviceManager->OnDualSenseConnected.AddUObject(this, &UBaseGameInstance::ApplyPlaystationVisuals);
		}
		else
		{
			DualSenseModule->DeviceManagerReady.AddUObject(this, &UBaseGameInstance::BindDualSenseEvents);
		}
	}

	USaveGame* Save = UGameplayStatics::LoadGameFromSlot(TEXT("Settings"), 0);
	if (!Save)
	{
		Save = UGameplayStatics::CreateSaveGameObject(USettingsSave::StaticClass());
		check(Save);
		SettingsSave = Cast<USettingsSave>(Save);
		check(SettingsSave);
		SettingsSave->InitializeFromDefaults(SettingsDataAsset);
		
	}
	check(Save)
	SettingsSave = Cast<USettingsSave>(Save);
	LoadRTPC(SettingsSave);
	
}

void UBaseGameInstance::BindDualSenseEvents()
{
	if (auto* DualSenseModule = &FWindowsDualsense_ds5wModule::Get())
	{
		if (auto DeviceManager = DualSenseModule->GetDeviceManager())
		{
			DeviceManager->OnDualSenseDisconnected.AddUObject(this, &UBaseGameInstance::ApplyGenericVisuals);
			DeviceManager->OnDualSenseConnected.AddUObject(this, &UBaseGameInstance::ApplyPlaystationVisuals);
		}
	}
}

class UTexture2D* UBaseGameInstance::GetCurrentControllerTypeTexture(int ID) const
{
	UTexture2D* NextTexture = nullptr;
	if (InputsTexturesPlatformsCopy)
	{
		for (auto InputTexture : InputsTexturesPlatformsCopy->AssociatedTextures)
		{
			if (InputTexture.ID == ID)
			{
				NextTexture = CurrentType == GENERIC ? InputTexture.Generic : InputTexture.Playstation;
				break;
			}
		}
	}
	return NextTexture;
}

void UBaseGameInstance::ApplyPlaystationVisuals()
{
	CurrentType = DUALSENSE;
	OnChangeControllerType.Broadcast();
}

void UBaseGameInstance::ApplyGenericVisuals()
{
	CurrentType = GENERIC;
	OnChangeControllerType.Broadcast();
}

void UBaseGameInstance::LoadRTPC_Implementation(USettingsSave* Save)
{
}

class USettingsSave* UBaseGameInstance::GetSettingsSave() const
{
	return SettingsSave;
}

int UBaseGameInstance::FindAssociatedInputMappingId(const FKey& Key) const
{
	int ID = 0;

	if (InputTexturesCopy)
	{
		for (auto InputTexture : InputTexturesCopy->InputsTextures)
		{
			if (InputTexture.Key == Key)
			{
				ID = InputTexture.Value;
			}
		}
	}
	return ID;
}
