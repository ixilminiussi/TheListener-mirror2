// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2025




#include "Core/Platforms/Windows/WindowsDeviceInfo.h"

#include "Runtime/ApplicationCore/Public/GenericPlatform/IInputInterface.h"
#include "Runtime/ApplicationCore/Public/GenericPlatform/GenericApplicationMessageHandler.h"

#include <hidsdi.h>
#include <setupapi.h>

void FWindowsDeviceInfo::Detect(TArray<FDeviceContext>& Devices)
{
	GUID HidGuid;
	HidD_GetHidGuid(&HidGuid);

	const HDEVINFO DeviceInfoSet = SetupDiGetClassDevs(&HidGuid, nullptr, nullptr,
	                                                   DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		UE_LOG(LogTemp, Error, TEXT("HIDManager: Falha ao obter informações dos dispositivos HID."));
		return;
	}

	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData = {};
	DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	TMap<int32, FString> DevicePaths;
	DevicePaths.Empty();
	for (int32 DeviceIndex = 0; SetupDiEnumDeviceInterfaces(DeviceInfoSet, nullptr, &HidGuid, DeviceIndex,
	                                                        &DeviceInterfaceData); DeviceIndex++)
	{
		DWORD RequiredSize = 0;
		SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, nullptr, 0, &RequiredSize, nullptr);

		const auto DetailDataBuffer = static_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(malloc(RequiredSize));
		if (!DetailDataBuffer)
		{
			UE_LOG(LogTemp, Error, TEXT("HIDManager: Failed to allocate memory for device details."));
			continue;
		}

		DetailDataBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		if (SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, DetailDataBuffer, RequiredSize,
		                                    nullptr, nullptr))
		{
			const HANDLE TempDeviceHandle = CreateFileW(
				DetailDataBuffer->DevicePath,
				GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr
			);
			
			if (
				TempDeviceHandle != INVALID_HANDLE_VALUE
			)
			{
				HIDD_ATTRIBUTES Attributes = {};
				Attributes.Size = sizeof(HIDD_ATTRIBUTES);

				if (HidD_GetAttributes(TempDeviceHandle, &Attributes))
				{
					if (
						Attributes.VendorID == 0x054C &&
						(
							Attributes.ProductID == 0x0CE6 ||
							Attributes.ProductID == 0x0DF2 ||
							Attributes.ProductID == 0x05C4 ||
							Attributes.ProductID == 0x09CC
						)
					)
					{
						FDeviceContext Context = {};
						WCHAR DeviceProductString[260];
						if (!HidD_GetProductString(TempDeviceHandle, DeviceProductString, 260))
						{
							UE_LOG(LogTemp, Error, TEXT("HIDManager: Failed to obtain device path for the DualSense."));
							continue;
						}

						if (DevicePaths.Contains(DeviceIndex))
						{
							continue;
						}

						FString PathStr(DetailDataBuffer->DevicePath);
						Context.Path = PathStr;
						DevicePaths.Add(DeviceIndex, PathStr);
						switch (Attributes.ProductID)
						{
							case 0x05C4:
							case 0x09CC:
								Context.DeviceType = DualShock4;
								break;
							case 0x0DF2:
								Context.DeviceType = DualSenseEdge;
								break;
							default: Context.DeviceType = DualSense;
						}
						
						Context.IsConnected = true;
						Context.ConnectionType = Usb;
						Context.Handle = TempDeviceHandle;
						FString DevicePath(Context.Path);
						if (DevicePath.Contains(TEXT("{00001124-0000-1000-8000-00805f9b34fb}")) ||
							DevicePath.Contains(TEXT("bth")) ||
							DevicePath.Contains(TEXT("BTHENUM")))
						{
							unsigned char FeatureBuffer[78];
							FeatureBuffer[0] = 0x05;
							if (!HidD_GetFeature(TempDeviceHandle, FeatureBuffer, 78)) {
								UE_LOG(LogTemp, Warning, TEXT("HIDManager: Failed to HidD_GetFeature."));
							}
							Context.ConnectionType = Bluetooth;
						}
						Devices.Add(Context);
					}
				}
				CloseHandle(TempDeviceHandle);
			}
		}
		free(DetailDataBuffer);
	}

	SetupDiDestroyDeviceInfoList(DeviceInfoSet);
}



void FWindowsDeviceInfo::Read(FDeviceContext* Context)
{
	if (!Context)
	{
		UE_LOG(LogTemp, Error, TEXT("Context nto found!"));
		return;
	}
	
	if (Context->Handle == INVALID_HANDLE_VALUE)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid device handle before attempting to read"));
		return;
	}
	
	if (!Context->IsConnected)
	{
		InvalidateHandle(Context);
		UE_LOG(LogTemp, Error, TEXT("Dualsense: DeviceContext->Connected, false"));
		return;
	}
	
	HidD_FlushQueue(Context->Handle);
	
	DWORD BytesRead = 0;
	if (Context->ConnectionType == Bluetooth && Context->DeviceType == EDeviceType::DualShock4)
	{
		const size_t InputReportLength = Context->ConnectionType == Bluetooth ? 547 : 64;
		if (sizeof(Context->BufferDS4) < InputReportLength)
		{
			InvalidateHandle(Context);
			return;
		}
		
		const EPollResult Response = PollTick(Context->Handle, Context->BufferDS4, InputReportLength, BytesRead);
		if (Response == EPollResult::Disconnected)
		{
			InvalidateHandle(Context);
		}
		return;
	}

	const size_t InputReportLength = Context->ConnectionType == Bluetooth ? 78 : 64;
	if (sizeof(Context->Buffer) < InputReportLength)
	{
		InvalidateHandle(Context);
		return;
	}

	const EPollResult Response = PollTick(Context->Handle, Context->Buffer, InputReportLength, BytesRead);
	if (Response == EPollResult::Disconnected)
	{
		InvalidateHandle(Context);
	}
}

void FWindowsDeviceInfo::Write(FDeviceContext* Context)
{
	if (Context->Handle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	
	size_t InReportLength = Context->DeviceType == DualShock4 ? 32 : 74;
	size_t OutputReportLength = Context->ConnectionType == Bluetooth ? 78 : InReportLength;

	DWORD BytesWritten = 0;
	if (!WriteFile(Context->Handle, Context->BufferOutput, OutputReportLength, &BytesWritten, nullptr))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed DualShock to write output data to device. report %llu error Code: %d"),
			OutputReportLength, GetLastError());
		InvalidateHandle(Context);
	}
}

bool FWindowsDeviceInfo::CreateHandle(FDeviceContext* DeviceContext)
{
	const HANDLE DeviceHandle = CreateFileW(
			*DeviceContext->Path,
			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr
		);

	if (DeviceHandle == INVALID_HANDLE_VALUE)
	{
		DeviceContext->Handle = DeviceHandle;
		UE_LOG(LogTemp, Error, TEXT("HIDManager: Failed to open device handle for the DualSense."));
		return false;
	}
	
	DeviceContext->Handle = DeviceHandle;
	return true;
}

void FWindowsDeviceInfo::InvalidateHandle(FDeviceContext* Context)
{
	IPlatformInputDeviceMapper::Get().Internal_SetInputDeviceConnectionState(Context->UniqueInputDeviceId, EInputDeviceConnectionState::Disconnected);
	if (!Context)
	{
		return;
	}
	
	if (Context->Handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(Context->Handle);
		Context->Handle = INVALID_HANDLE_VALUE;
		Context->IsConnected = false;
		Context->Path = nullptr;
		
		ZeroMemory(Context->Buffer, sizeof(Context->Buffer));
		ZeroMemory(Context->BufferDS4, sizeof(Context->BufferDS4));
		ZeroMemory(Context->BufferOutput, sizeof(Context->BufferOutput));
		UE_LOG(LogTemp, Log, TEXT("HIDManager: Invalidate Handle."));
	}
}

void FWindowsDeviceInfo::InvalidateHandle(HANDLE Handle)
{
	if (Handle != INVALID_PLATFORM_HANDLE)
	{
		CloseHandle(Handle);
		UE_LOG(LogTemp, Log, TEXT("HIDManager: Invalidate Handle."));
	}
}

EPollResult FWindowsDeviceInfo::PollTick(HANDLE Handle, unsigned char* Buffer, int32 Length, DWORD& OutBytesRead)
{
	int32 Err = ERROR_SUCCESS;
	PingOnce(Handle, &Err);
	
	OutBytesRead = 0;
	if (!ReadFile(Handle, Buffer, Length, &OutBytesRead, nullptr))
	{
		const int32 Error = GetLastError();
		if (ShouldTreatAsDisconnected(Error))
		{
			return EPollResult::Disconnected;
		}

		InvalidateHandle(Handle);
	}

	return EPollResult::ReadOk;
}


bool FWindowsDeviceInfo::PingOnce(HANDLE Handle, int32* OutLastError)
{
	FILE_STANDARD_INFO Info{};
	if (!GetFileInformationByHandleEx(Handle, FileStandardInfo, &Info, sizeof(Info)))
	{
		if (OutLastError) *OutLastError = GetLastError();
		return false;
	}
	if (OutLastError) *OutLastError = ERROR_SUCCESS;
	return true;
}
