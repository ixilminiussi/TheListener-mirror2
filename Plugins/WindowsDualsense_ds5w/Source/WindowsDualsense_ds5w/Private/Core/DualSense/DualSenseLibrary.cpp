// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2025

#include "Core/DualSense/DualSenseLibrary.h"
#include "Async/Async.h"
#include "Async/TaskGraphInterfaces.h"
#include "InputCoreTypes.h"
#include "Helpers/ValidateHelpers.h"
#include "Core/Interfaces/PlatformHardwareInfoInterface.h"
#include "Core/PlayStationOutputComposer.h"
#include "Core/Structs/FOutputContext.h"

bool UDualSenseLibrary::InitializeLibrary(const FDeviceContext& Context)
{
	HIDDeviceContexts = Context;
	if (HIDDeviceContexts.ConnectionType == Bluetooth)
	{
		FOutputContext* EnableReport = &HIDDeviceContexts.Output; // Create a clean/empty report

		// HidOutput->Feature.FeatureMode = 0x1 | 0x2 | 0x4 | 0x8 | 0x10 | 0x40;
		// Set flags to enable control over the lightbar, player LEDs, etc.
		EnableReport->Feature.FeatureMode = 0x04 | 0x08;

		// Important: Keep the data fields at their default (e.g., lights off)
		EnableReport->Lightbar = {0, 0, 0};
		EnableReport->PlayerLed.Brightness = 0x00;

		SendOut();
		// Small delay here is sometimes needed, but often not.
		FPlatformProcess::Sleep(0.01f);
	}

	StopAll();
	return true;
}

void UDualSenseLibrary::ShutdownLibrary()
{
	ButtonStates.Reset();
	FPlayStationOutputComposer::FreeContext(&HIDDeviceContexts);
}

bool UDualSenseLibrary::IsConnected()
{
	return HIDDeviceContexts.IsConnected;
}

void UDualSenseLibrary::SendOut()
{
	if (!HIDDeviceContexts.IsConnected)
	{
		return;
	}

	FPlayStationOutputComposer::OutputDualSense(&HIDDeviceContexts);
}

void UDualSenseLibrary::Settings(const FSettings<FFeatureReport>& Settings)
{
}

void UDualSenseLibrary::Settings(const FDualSenseFeatureReport& Settings)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	HidOutput->Feature.VibrationMode = Settings.VibrationMode == EDualSenseDeviceFeatureReport::Off
		                                   ? 0xFF
		                                   : static_cast<uint8_t>(Settings.VibrationMode);
	HidOutput->Feature.SoftRumbleReduce = static_cast<uint8_t>(Settings.SoftRumbleReduce);
	HidOutput->Feature.TriggerSoftnessLevel = static_cast<uint8_t>(Settings.TriggerSoftnessLevel);

	HidOutput->Audio.MicStatus = static_cast<uint8_t>(Settings.MicStatus);
	HidOutput->Audio.MicVolume = static_cast<uint8_t>(Settings.MicVolume);
	HidOutput->Audio.HeadsetVolume = static_cast<uint8_t>(Settings.AudioVolume);
	HidOutput->Audio.SpeakerVolume = static_cast<uint8_t>(Settings.AudioVolume);

	if (Settings.AudioHeadset == EDualSenseAudioFeatureReport::On && Settings.AudioSpeaker ==
		EDualSenseAudioFeatureReport::Off)
	{
		HidOutput->Audio.Mode = 0x05;
	}

	if (Settings.AudioHeadset == EDualSenseAudioFeatureReport::On && Settings.AudioSpeaker ==
		EDualSenseAudioFeatureReport::On)
	{
		HidOutput->Audio.Mode = 0x21;
	}

	if (Settings.AudioHeadset == EDualSenseAudioFeatureReport::Off && Settings.AudioSpeaker ==
		EDualSenseAudioFeatureReport::On)
	{
		HidOutput->Audio.Mode = 0x31;
	}

	SendOut();
}

void UDualSenseLibrary::CheckButtonInput(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler,
                                         const FPlatformUserId UserId, const FInputDeviceId InputDeviceId,
                                         const FName ButtonName, const bool IsButtonPressed)
{
	const bool PreviousState = ButtonStates.Contains(ButtonName) ? ButtonStates[ButtonName] : false;
	if (IsButtonPressed && !PreviousState)
	{
		InMessageHandler.Get().OnControllerButtonPressed(ButtonName, UserId, InputDeviceId, false);
	}

	if (!IsButtonPressed && PreviousState)
	{
		InMessageHandler.Get().OnControllerButtonReleased(ButtonName, UserId, InputDeviceId, false);
	}

	ButtonStates.Add(ButtonName, IsButtonPressed);
}

void UDualSenseLibrary::UpdateInput(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler,
                                    const FPlatformUserId UserId, const FInputDeviceId InputDeviceId, float Delta)
{
	FDeviceContext* Context = &HIDDeviceContexts;
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [NewContext = MoveTemp(Context)]()
	{
		IPlatformHardwareInfoInterface::Get().Read(NewContext);
	});

	const size_t Padding = HIDDeviceContexts.ConnectionType == Bluetooth ? 2 : 1;
	const unsigned char* HIDInput = &HIDDeviceContexts.Buffer[Padding];

	const float LeftAnalogX = static_cast<char>(static_cast<short>(HIDInput[0x00] - 128));
	const float LeftAnalogY = static_cast<char>(static_cast<short>(HIDInput[0x01] - 127) * -1);
	InMessageHandler.Get().OnControllerAnalog(FGamepadKeyNames::LeftAnalogX, UserId, InputDeviceId,
	                                          LeftAnalogX / 128.0f);
	InMessageHandler.Get().OnControllerAnalog(FGamepadKeyNames::LeftAnalogY, UserId, InputDeviceId,
	                                          LeftAnalogY / 128.0f);

	const float RightAnalogX = static_cast<char>(static_cast<short>(HIDInput[0x02] - 128));
	const float RightAnalogY = static_cast<char>(static_cast<short>(HIDInput[0x03] - 127) * -1);
	InMessageHandler.Get().OnControllerAnalog(FGamepadKeyNames::RightAnalogX, UserId, InputDeviceId,
	                                          RightAnalogX / 128.0f);
	InMessageHandler.Get().OnControllerAnalog(FGamepadKeyNames::RightAnalogY, UserId, InputDeviceId,
	                                          RightAnalogY / 128.0f);

	const float TriggerL = HIDInput[0x04] / 256.0f;
	const float TriggerR = HIDInput[0x05] / 256.0f;
	InMessageHandler.Get().OnControllerAnalog(FGamepadKeyNames::LeftTriggerAnalog, UserId, InputDeviceId, TriggerL);
	InMessageHandler.Get().OnControllerAnalog(FGamepadKeyNames::RightTriggerAnalog, UserId, InputDeviceId, TriggerR);

	uint8_t ButtonsMask = HIDInput[0x07] & 0xF0;
	const bool bCross = ButtonsMask & BTN_CROSS;
	const bool bSquare = ButtonsMask & BTN_SQUARE;
	const bool bCircle = ButtonsMask & BTN_CIRCLE;
	const bool bTriangle = ButtonsMask & BTN_TRIANGLE;

	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::FaceButtonBottom, bCross);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::FaceButtonLeft, bSquare);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::FaceButtonRight, bCircle);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::FaceButtonTop, bTriangle);

	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::RightStickUp, RightAnalogY / 128.0f > AnalogDeadZone);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::RightStickDown, RightAnalogY / 128.0f < -AnalogDeadZone);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::RightStickLeft, RightAnalogX / 128.0f < -AnalogDeadZone);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::RightStickRight, RightAnalogX / 128.0f > AnalogDeadZone);

	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::LeftStickUp, LeftAnalogY / 128.0f > AnalogDeadZone);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::LeftStickDown, LeftAnalogY / 128.0f < -AnalogDeadZone);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::LeftStickLeft, LeftAnalogX / 128.0f < -AnalogDeadZone);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::LeftStickRight, LeftAnalogX / 128.0f > AnalogDeadZone);

	switch (HIDInput[0x07] & 0x0F)
	{
	case 0x0:
		ButtonsMask |= BTN_DPAD_UP;
		break;
	case 0x4:
		ButtonsMask |= BTN_DPAD_DOWN;
		break;
	case 0x6:
		ButtonsMask |= BTN_DPAD_LEFT;
		break;
	case 0x2:
		ButtonsMask |= BTN_DPAD_RIGHT;
		break;
	case 0x5:
		ButtonsMask |= BTN_DPAD_LEFT | BTN_DPAD_DOWN;
		break;
	case 0x7:
		ButtonsMask |= BTN_DPAD_LEFT | BTN_DPAD_UP;
		break;
	case 0x1:
		ButtonsMask |= BTN_DPAD_RIGHT | BTN_DPAD_UP;
		break;
	case 0x3:
		ButtonsMask |= BTN_DPAD_RIGHT | BTN_DPAD_DOWN;
		break;
	default: ;
	}
	const bool bDPadLeft = ButtonsMask & BTN_DPAD_LEFT;
	const bool bDPadDown = ButtonsMask & BTN_DPAD_DOWN;
	const bool bDPadRight = ButtonsMask & BTN_DPAD_RIGHT;
	const bool bDPadUp = ButtonsMask & BTN_DPAD_UP;

	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::DPadUp, bDPadUp);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::DPadDown, bDPadDown);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::DPadLeft, bDPadLeft);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::DPadRight, bDPadRight);

	// Shoulders
	const bool bLeftShoulder = HIDInput[0x08] & BTN_LEFT_SHOLDER;
	const bool bRightShoulder = HIDInput[0x08] & BTN_RIGHT_SHOLDER;

	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::LeftShoulder, bLeftShoulder);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::RightShoulder, bRightShoulder);

	// Push Stick
	const bool PushLeftStick = HIDInput[0x08] & BTN_LEFT_STICK;
	const bool PushRightStick = HIDInput[0x08] & BTN_RIGHT_STICK;
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FName("PS_PushLeftStick"), PushLeftStick);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FName("PS_PushRightStick"), PushRightStick);

	// mapped urenal native gamepad Push Stick
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::LeftThumb, PushLeftStick);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::RightThumb, PushRightStick);

	// Function & Special Actions
	const bool Playstation = HIDInput[0x09] & BTN_PLAYSTATION_LOGO;
	const bool TouchPad = HIDInput[0x09] & BTN_PAD_BUTTON;
	const bool Mic = HIDInput[0x09] & BTN_MIC_BUTTON;
	const bool bFn1 = HIDInput[0x09] & BTN_FN1;
	const bool bFn2 = HIDInput[0x09] & BTN_FN2;
	const bool bPaddleLeft = HIDInput[0x09] & BTN_PADDLE_LEFT;
	const bool bPaddleRight = HIDInput[0x09] & BTN_PADDLE_RIGHT;

	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FName("PS_Mic"), Mic);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FName("PS_TouchButtom"), TouchPad);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FName("PS_Button"), Playstation);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FName("PS_FunctionL"), bFn1);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FName("PS_FunctionR"), bFn2);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FName("PS_PaddleL"), bPaddleLeft);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FName("PS_PaddleR"), bPaddleRight);

	const bool Start = HIDInput[0x08] & BTN_START;
	const bool Select = HIDInput[0x08] & BTN_SELECT;
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FName("PS_Menu"), Start);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FName("PS_Share"), Select);

	// mapped urenal native gamepad Start and Select
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::SpecialRight, Start);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::SpecialLeft, Select);


	const bool bLeftTriggerThreshold = HIDInput[0x08] & BTN_LEFT_TRIGGER;
	const bool bRightTriggerThreshold = HIDInput[0x08] & BTN_RIGHT_TRIGGER;
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::LeftTriggerThreshold,
	                 bLeftTriggerThreshold);
	CheckButtonInput(InMessageHandler, UserId, InputDeviceId, FGamepadKeyNames::RightTriggerThreshold,
	                 bRightTriggerThreshold);
	if (bEnableTouch)
	{
		FTouchPoint1 Touch;
		const int32 Touchpad1Raw = *reinterpret_cast<const int32*>(&HIDInput[0x20]);
		Touch.Y = (Touchpad1Raw & 0xFFF00000) >> 20;
		Touch.X = (Touchpad1Raw & 0x000FFF00) >> 8;
		Touch.Down = (Touchpad1Raw & (1 << 7)) == 0;
		Touch.Id = (Touchpad1Raw & 127) % 10;

		bool bIsTouchDown = Touch.Down;
		if (bIsTouchDown) // pressed
		{
			if (!bWasTouch1Down)
			{
				const FVector2D TouchVectorStart = FVector2D(Touch.X, Touch.Y);
				InMessageHandler->OnTouchStarted(
					nullptr,
					TouchVectorStart,
					1.0f,
					Touch.Id,
					UserId,
					InputDeviceId
				);
			}
			else
			{
				const FVector2D TouchVector = FVector2D(Touch.X, Touch.Y);
				InMessageHandler->OnTouchMoved(TouchVector, 1.0f, Touch.Id, UserId, InputDeviceId);
			}
		}
		else if (!bIsTouchDown && bWasTouch1Down)
		{
			const FVector2D TouchVectorEnded = FVector2D(Touch.X, Touch.Y);
			InMessageHandler->OnTouchEnded(
				TouchVectorEnded,
				Touch.Id,
				UserId,
				InputDeviceId
			);
		}

		bWasTouch1Down = bIsTouchDown;

		// // Evaluate touch state 2
		FTouchPoint2 Touch2;
		const int32 Touchpad2Raw = *reinterpret_cast<const int32*>(&HIDInput[0x24]);
		Touch2.Y = (Touchpad2Raw & 0xFFF00000) >> 20;
		Touch2.X = (Touchpad2Raw & 0x000FFF00) >> 8;
		Touch2.Down = (Touchpad2Raw & (1 << 7)) == 0;
		Touch2.Id = (Touchpad2Raw & 127) % 10;

		bool bIsTouch2Down = Touch2.Down;
		if (bIsTouch2Down)
		{
			if (!bWasTouch2Down)
			{
				const FVector2D Touch2VectorStart = FVector2D(Touch2.X, Touch2.Y);
				InMessageHandler->OnTouchStarted(nullptr, Touch2VectorStart, 1.0f, Touch2.Id, UserId, InputDeviceId);
			}
			else
			{
				const FVector2D Touch2Vector = FVector2D(Touch2.X, Touch2.Y);
				InMessageHandler->OnTouchMoved(Touch2Vector, 1.0f, Touch2.Id, UserId, InputDeviceId);
			}
		}
		else if (!bIsTouch2Down && bWasTouch2Down)
		{
			const FVector2D Touch2VectorEnded = FVector2D(Touch2.X, Touch2.Y);
			InMessageHandler->OnTouchEnded(
				Touch2VectorEnded,
				Touch2.Id,
				UserId,
				InputDeviceId
			);
		}

		bWasTouch2Down = bIsTouch2Down;
	}

	if (bEnableAccelerometerAndGyroscope)
	{
		FGyro Gyro;
		Gyro.X = static_cast<int16_t>((HIDInput[16]) | (HIDInput[17] << 8));
		Gyro.Y = static_cast<int16_t>((HIDInput[18]) | (HIDInput[19] << 8));
		Gyro.Z = static_cast<int16_t>((HIDInput[20]) | (HIDInput[21] << 8));

		FAccelerometer Acc;
		Acc.X = static_cast<int16_t>((HIDInput[22]) | (HIDInput[23] << 8));
		Acc.Y = static_cast<int16_t>((HIDInput[24]) | (HIDInput[25] << 8));
		Acc.Z = static_cast<int16_t>((HIDInput[26]) | (HIDInput[27] << 8));

		if (bIsCalibrating)
		{
			AccumulatedGyro.X += Gyro.X;
			AccumulatedGyro.Y += Gyro.Y;
			AccumulatedGyro.Z += Gyro.Z;

			AccumulatedAccel.X += Acc.X;
			AccumulatedAccel.Y += Acc.Y;
			AccumulatedAccel.Z += Acc.Z;


			Bounds.Gyro_X_Bounds.X = FMath::Min(Bounds.Gyro_X_Bounds.X, Gyro.X);
			Bounds.Gyro_X_Bounds.Y = FMath::Max(Bounds.Gyro_X_Bounds.Y, Gyro.X);

			Bounds.Gyro_Y_Bounds.X = FMath::Min(Bounds.Gyro_Y_Bounds.X, Gyro.Y);
			Bounds.Gyro_Y_Bounds.Y = FMath::Max(Bounds.Gyro_Y_Bounds.Y, Gyro.Y);

			Bounds.Gyro_Z_Bounds.X = FMath::Min(Bounds.Gyro_Z_Bounds.X, Gyro.Z);
			Bounds.Gyro_Z_Bounds.Y = FMath::Max(Bounds.Gyro_Z_Bounds.Y, Gyro.Z);

			Bounds.Accel_X_Bounds.X = FMath::Min(Bounds.Accel_X_Bounds.X, Acc.X);
			Bounds.Accel_X_Bounds.Y = FMath::Max(Bounds.Accel_X_Bounds.Y, Acc.X);

			Bounds.Accel_Y_Bounds.X = FMath::Min(Bounds.Accel_Y_Bounds.X, Acc.Y);
			Bounds.Accel_Y_Bounds.Y = FMath::Max(Bounds.Accel_Y_Bounds.Y, Acc.Y);

			Bounds.Accel_Z_Bounds.X = FMath::Min(Bounds.Accel_Z_Bounds.X, Acc.Z);
			Bounds.Accel_Z_Bounds.Y = FMath::Max(Bounds.Accel_Z_Bounds.Y, Acc.Z);

			CalibrationSampleCount++;
		}

		if (bHasMotionSensorBaseline)
		{
			Gyro.X -= GyroBaseline.X;
			Gyro.Y -= GyroBaseline.Y;
			Gyro.Z -= GyroBaseline.Z;
			
			float FinalGyroValueX = 0.0f;
			if (FMath::Abs(Gyro.X) > (Bounds.Gyro_X_Bounds.Y - Bounds.Gyro_X_Bounds.X) * SensorsDeadZone)
			{
				FinalGyroValueX = Gyro.X;
			}

			float FinalGyroValueY = 0.0f;
			if (FMath::Abs(Gyro.Y) > (Bounds.Gyro_Y_Bounds.Y - Bounds.Gyro_Y_Bounds.X) * SensorsDeadZone)
			{
				FinalGyroValueY = Gyro.Y;
			}

			float FinalGyroValueZ = 0.0f;
			if (FMath::Abs(Gyro.Z) > (Bounds.Gyro_Z_Bounds.Y - Bounds.Gyro_Z_Bounds.X) * SensorsDeadZone)
			{
				FinalGyroValueZ = Gyro.Z;
			}

			Acc.X -= AccelBaseline.X;
			Acc.Y -= AccelBaseline.Y;
			Acc.Z -= AccelBaseline.Z;

			float FinalAccelValueX = 0.0f;
			if (FMath::Abs(Acc.X) > (Bounds.Accel_X_Bounds.Y - Bounds.Accel_X_Bounds.X) * SensorsDeadZone)
			{
				FinalAccelValueX = Acc.X;
			}

			float FinalAccelValueY = 0.0f;
			if (FMath::Abs(Acc.Y) > (Bounds.Accel_Y_Bounds.Y - Bounds.Accel_Y_Bounds.X) * SensorsDeadZone)
			{
				FinalAccelValueY = Acc.Y;
			}

			float FinalAccelValueZ = 0.0f;
			if (FMath::Abs(Acc.Z) > (Bounds.Accel_Z_Bounds.Y - Bounds.Accel_Z_Bounds.X) *
				SensorsDeadZone)
			{
				FinalAccelValueZ = Acc.Z;
			}

			Gyro.X = FinalGyroValueX;
			Gyro.Y = FinalGyroValueY;
			Gyro.Z = FinalGyroValueZ;

			Acc.X = FinalAccelValueX;
			Acc.Y = FinalAccelValueY;
			Acc.Z = FinalAccelValueZ;

			FRotator GyroDelta(
			Gyro.X * Delta,
				Gyro.Z * Delta,
				Gyro.Y * Delta
			);
			FQuat GyroBasedOrientation = FusedOrientation * GyroDelta.Quaternion();
			FVector AccelDirection = FVector(Acc.X ,  Acc.Z, Acc.Y).GetSafeNormal();
			FQuat AccelBasedOrientation = AccelDirection.ToOrientationQuat();
			FusedOrientation = FQuat::Slerp(AccelBasedOrientation, GyroBasedOrientation,  0.98f);
			const FRotator FusedOrientationRotator = FusedOrientation.Rotator();
			const FVector Tilt = FVector( FusedOrientationRotator.Pitch, FusedOrientationRotator.Yaw, FusedOrientationRotator.Roll);
			
			const FVector Gyroscope = FVector(Gyro.X, Gyro.Z, Gyro.Y);
			const FVector Accelerometer = FVector(Acc.X, Acc.Z, Acc.Y);
			const float GravityMagnitude = FVector(Acc.X, Acc.Z, Acc.Y).Size();
			const FVector Gravity = (FVector(Acc.X, Acc.Z, Acc.Y) / GravityMagnitude) * 9.81f;
			InMessageHandler.Get().OnMotionDetected(Tilt, Gyroscope, Gravity, Accelerometer, UserId, InputDeviceId);
		}
	}

	SetHasPhoneConnected(HIDInput[0x35] & 0x01);
	SetLevelBattery(((HIDInput[0x34] & 0x0F) * 100) / 8, (HIDInput[0x35] & 0x00), (HIDInput[0x36] & 0x20));
}

void UDualSenseLibrary::SetVibration(const FForceFeedbackValues& Vibration)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	const float LeftRumble = FMath::Max(Vibration.LeftLarge, Vibration.LeftSmall);
	const float RightRumble = FMath::Max(Vibration.RightLarge, Vibration.RightSmall);

	const unsigned char OutputLeft = static_cast<unsigned char>(FValidateHelpers::To255(LeftRumble));
	const unsigned char OutputRight = static_cast<unsigned char>(FValidateHelpers::To255(RightRumble));
	if (HidOutput->Rumbles.Left != OutputLeft || HidOutput->Rumbles.Right != OutputRight)
	{
		HidOutput->Rumbles = {OutputLeft, OutputRight};
		SendOut();
	}
}

void UDualSenseLibrary::SetVibrationAudioBased(
	const FForceFeedbackValues& Vibration,
	const float Threshold = 0.015f,
	const float ExponentCurve = 2.f,
	const float BaseMultiplier = 1.5f
)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	const float InputLeft = FMath::Max(Vibration.LeftLarge, Vibration.LeftSmall);
	const float InputRight = FMath::Max(Vibration.RightLarge, Vibration.RightSmall);

	float IntensityLeftRumble = 0.0f;
	if (InputLeft >= Threshold)
	{
		IntensityLeftRumble = BaseMultiplier *
			FMath::Pow((InputLeft - Threshold) / (1.0f - Threshold), ExponentCurve);
	}

	float IntensityRightRumble = 0.0f;
	if (InputRight >= Threshold)
	{
		IntensityRightRumble = BaseMultiplier *
			FMath::Pow((InputRight - Threshold) / (1.0f - Threshold), ExponentCurve);
	}

	const unsigned char OutputLeft = static_cast<unsigned char>(FValidateHelpers::To255(IntensityLeftRumble));
	const unsigned char OutputRight = static_cast<unsigned char>(FValidateHelpers::To255(IntensityRightRumble));
	HidOutput->Rumbles = {OutputLeft, OutputRight};

	SendOut();
}

void UDualSenseLibrary::SetHapticFeedback(int32 Hand, const FHapticFeedbackValues* Values)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	if (Hand == static_cast<int32>(EControllerHand::Left) || Hand == static_cast<int32>(EControllerHand::AnyHand))
	{
		HidOutput->LeftTrigger.Frequency = FValidateHelpers::To255(Values->Frequency);
	}

	if (Hand == static_cast<int32>(EControllerHand::Right) || Hand == static_cast<int32>(EControllerHand::AnyHand))
	{
		HidOutput->RightTrigger.Frequency = FValidateHelpers::To255(Values->Frequency);
	}

	SendOut();
}

void UDualSenseLibrary::SetTriggers(const FInputDeviceProperty* Values)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	if (Values->Name == FName("InputDeviceTriggerResistance"))
	{
		const FInputDeviceTriggerResistanceProperty* Resistance = static_cast<const
			FInputDeviceTriggerResistanceProperty*>(Values);

		const uint8_t Start = Resistance->StartPosition;
		const uint8_t End = Resistance->EndPosition;
		const float StartStr = static_cast<float>(Resistance->StartStrengh);
		const float EndStr = static_cast<float>(Resistance->EndStrengh);

		constexpr int NumZones = 10;
		uint8_t Strengths[NumZones] = {0};
		if (End > Start)
		{
			for (int i = Start; i <= End && i < NumZones; ++i)
			{
				const float Alpha = (End == Start) ? 0.0f : (i - Start) / (End - Start);
				Strengths[i] = static_cast<uint8>(StartStr + Alpha * (EndStr - StartStr));
			}
		}


		int32 ActiveZones = 0;
		int64 StrengthZones = 0;
		for (int i = 0; i < 10; i++)
		{
			if (Strengths[i] > 0)
			{
				const uint64_t StrengthValue = static_cast<uint64_t>((Strengths[i] - 1) & 0x07);
				StrengthZones |= static_cast<int64>(StrengthValue << (3 * i));
				ActiveZones |= (1 << i);
			}
		}

		if (
			Resistance->AffectedTriggers == EInputDeviceTriggerMask::Left ||
			Resistance->AffectedTriggers == EInputDeviceTriggerMask::All
		)
		{
			HidOutput->LeftTrigger.Mode = 0x02;
			HidOutput->LeftTrigger.Strengths.ActiveZones = ActiveZones;
			HidOutput->LeftTrigger.Strengths.StrengthZones = StrengthZones;
		}

		if (
			Resistance->AffectedTriggers == EInputDeviceTriggerMask::Right ||
			Resistance->AffectedTriggers == EInputDeviceTriggerMask::All
		)
		{
			HidOutput->RightTrigger.Mode = 0x02;
			HidOutput->RightTrigger.Strengths.ActiveZones = ActiveZones;
			HidOutput->RightTrigger.Strengths.StrengthZones = StrengthZones;
		}
	}

	SendOut();
}

void UDualSenseLibrary::SetAutomaticGun(int32 BeginStrength, int32 MiddleStrength, int32 EndStrength,
                                        const EControllerHand& Hand, bool KeepEffect, float Frequency)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	unsigned char PositionalAmplitudes[10];
	PositionalAmplitudes[0] = BeginStrength;
	PositionalAmplitudes[1] = BeginStrength;
	PositionalAmplitudes[2] = BeginStrength;
	PositionalAmplitudes[3] = BeginStrength;
	PositionalAmplitudes[4] = MiddleStrength;
	PositionalAmplitudes[5] = MiddleStrength;
	PositionalAmplitudes[6] = MiddleStrength;
	PositionalAmplitudes[7] = MiddleStrength;
	PositionalAmplitudes[8] = KeepEffect ? 8 : EndStrength;
	PositionalAmplitudes[9] = KeepEffect ? 8 : EndStrength;

	unsigned char Strengths[10];
	for (int i = 0; i < 10; i++)
	{
		Strengths[i] = static_cast<uint64_t>(PositionalAmplitudes[i] * 8.0f);
	}

	int64 StrengthZones = 0;
	int32 ActiveZones = 0;
	for (int i = 0; i < 10; i++)
	{
		if (PositionalAmplitudes[i] > 0)
		{
			const uint64_t StrengthValue = static_cast<uint64_t>((Strengths[i] - 1) & 0x07);
			StrengthZones |= static_cast<int64>(StrengthValue << (3 * i));
			ActiveZones |= (1 << i);
		}
	}

	if (Hand == EControllerHand::Left || Hand == EControllerHand::AnyHand)
	{
		HidOutput->LeftTrigger.Mode = 0x26;
		HidOutput->LeftTrigger.Strengths.ActiveZones = ActiveZones;
		HidOutput->LeftTrigger.Strengths.StrengthZones = StrengthZones;
		HidOutput->LeftTrigger.Frequency = FValidateHelpers::To255(Frequency);
	}

	if (Hand == EControllerHand::Right || Hand == EControllerHand::AnyHand)
	{
		HidOutput->RightTrigger.Mode = 0x26;
		HidOutput->RightTrigger.Strengths.ActiveZones = ActiveZones;
		HidOutput->RightTrigger.Strengths.StrengthZones = StrengthZones;
		HidOutput->RightTrigger.Frequency = FValidateHelpers::To255(Frequency);
	}

	SendOut();
}

void UDualSenseLibrary::SetContinuousResistance(int32 StartPosition, int32 Strength, const EControllerHand& Hand)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	if (Hand == EControllerHand::Left || Hand == EControllerHand::AnyHand)
	{
		HidOutput->LeftTrigger.Mode = 0x01;
		HidOutput->LeftTrigger.Strengths.ActiveZones = FValidateHelpers::To255(StartPosition, 8);
		HidOutput->LeftTrigger.Strengths.StrengthZones = FValidateHelpers::To255(Strength, 9);
	}

	if (Hand == EControllerHand::Right || Hand == EControllerHand::AnyHand)
	{
		HidOutput->RightTrigger.Mode = 0x01;
		HidOutput->RightTrigger.Strengths.ActiveZones = FValidateHelpers::To255(StartPosition, 8);
		HidOutput->RightTrigger.Strengths.StrengthZones = FValidateHelpers::To255(Strength, 9);
	}

	SendOut();
}

void UDualSenseLibrary::SetResistance(int32 BeginStrength, int32 MiddleStrength, int32 EndStrength,
                                      const EControllerHand& Hand)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	unsigned char PositionalAmplitudes[10];
	PositionalAmplitudes[0] = BeginStrength;
	PositionalAmplitudes[1] = BeginStrength;
	PositionalAmplitudes[2] = BeginStrength;
	PositionalAmplitudes[3] = BeginStrength;
	PositionalAmplitudes[4] = MiddleStrength;
	PositionalAmplitudes[5] = MiddleStrength;
	PositionalAmplitudes[6] = MiddleStrength;
	PositionalAmplitudes[7] = MiddleStrength;
	PositionalAmplitudes[8] = EndStrength;
	PositionalAmplitudes[9] = EndStrength;

	int32 ActiveZones = 0;
	int16 StrengthValues = 0;
	for (int i = 0; i < 3; i++)
	{
		if (PositionalAmplitudes[i] > 0)
		{
			const int8_t StrengthValue = static_cast<int8_t>((PositionalAmplitudes[i] - 1) & 0x07);
			StrengthValues |= (StrengthValue << (3 * i));
			ActiveZones |= static_cast<int16>(1 << i);
		}
	}

	if (Hand == EControllerHand::Left || Hand == EControllerHand::AnyHand)
	{
		HidOutput->LeftTrigger.Mode = 0x21;
		HidOutput->LeftTrigger.Strengths.ActiveZones = ActiveZones;
		HidOutput->LeftTrigger.Strengths.StrengthZones = StrengthValues;
	}

	if (Hand == EControllerHand::Right || Hand == EControllerHand::AnyHand)
	{
		HidOutput->RightTrigger.Mode = 0x21;
		HidOutput->RightTrigger.Strengths.ActiveZones = ActiveZones;
		HidOutput->RightTrigger.Strengths.StrengthZones = StrengthValues;
	}

	SendOut();
}

void UDualSenseLibrary::SetWeapon(int32 StartPosition, int32 EndPosition, int32 Strength,
                                  const EControllerHand& Hand)
{
	const uint32_t ActiveZones = (1 << StartPosition) | (1 << EndPosition);
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	if (Hand == EControllerHand::Left || Hand == EControllerHand::AnyHand)
	{
		HidOutput->LeftTrigger.Mode = 0x25;
		HidOutput->LeftTrigger.Strengths.ActiveZones = ActiveZones;
		HidOutput->LeftTrigger.Strengths.StrengthZones = FValidateHelpers::To255(Strength);
	}

	if (Hand == EControllerHand::Right || Hand == EControllerHand::AnyHand)
	{
		HidOutput->RightTrigger.Mode = 0x25;
		HidOutput->RightTrigger.Strengths.ActiveZones = ActiveZones;
		HidOutput->RightTrigger.Strengths.StrengthZones = FValidateHelpers::To255(Strength);
	}

	SendOut();
}

void UDualSenseLibrary::SetGalloping(int32 StartPosition, int32 EndPosition, int32 FirstFoot, int32 SecondFoot,
                                     float Frequency, const EControllerHand& Hand)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	const uint32_t ActiveZones = (1 << StartPosition) | (1 << EndPosition);
	const uint32_t TimeAndRatio = (SecondFoot & 0x07) << (3 * 0) | (FirstFoot & 0x07);
	if (Hand == EControllerHand::Left || Hand == EControllerHand::AnyHand)
	{
		HidOutput->LeftTrigger.Mode = 0x23;
		HidOutput->LeftTrigger.Strengths.ActiveZones = ActiveZones;
		HidOutput->LeftTrigger.Strengths.TimeAndRatio = TimeAndRatio;
		HidOutput->LeftTrigger.Frequency = FValidateHelpers::To255(Frequency);
	}

	if (Hand == EControllerHand::Right || Hand == EControllerHand::AnyHand)
	{
		HidOutput->RightTrigger.Mode = 0x23;
		HidOutput->RightTrigger.Strengths.ActiveZones = ActiveZones;
		HidOutput->RightTrigger.Strengths.TimeAndRatio = TimeAndRatio;
		HidOutput->RightTrigger.Frequency = FValidateHelpers::To255(Frequency);
	}

	SendOut();
}

void UDualSenseLibrary::SetMachine(int32 StartPosition, int32 EndPosition, int32 AmplitudeBegin,
                                   int32 AmplitudeEnd, float Frequency, float Period,
                                   const EControllerHand& Hand)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	const uint32_t ActiveZones = ((1 << StartPosition) | (1 << EndPosition));
	const uint32_t Strengths = (((AmplitudeBegin & 0x07) << (3 * 0)) | ((AmplitudeEnd & 0x07) << (3 * 1)));

	if (Period < 0.0f || Period > 3.f)
	{
		Period = 3.f;
	}

	if (Hand == EControllerHand::Left || Hand == EControllerHand::AnyHand)
	{
		HidOutput->LeftTrigger.Mode = 0x27;
		HidOutput->LeftTrigger.Strengths.ActiveZones = ActiveZones;
		HidOutput->LeftTrigger.Strengths.StrengthZones = Strengths;
		HidOutput->LeftTrigger.Strengths.Period = FValidateHelpers::To255(Period);
		HidOutput->LeftTrigger.Frequency = FValidateHelpers::To255(Frequency);
	}

	if (Hand == EControllerHand::Right || Hand == EControllerHand::AnyHand)
	{
		HidOutput->RightTrigger.Mode = 0x27;
		HidOutput->RightTrigger.Strengths.ActiveZones = ActiveZones;
		HidOutput->RightTrigger.Strengths.StrengthZones = Strengths;
		HidOutput->RightTrigger.Strengths.Period = FValidateHelpers::To255(Period);
		HidOutput->RightTrigger.Frequency = FValidateHelpers::To255(Frequency);
	}

	SendOut();
}

void UDualSenseLibrary::SetBow(int32 StartPosition, int32 EndPosition, int32 BegingStrength, int32 EndStrength,
                               const EControllerHand& Hand)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	const uint32_t ActiveZones = ((1 << StartPosition) | (1 << EndPosition));
	const uint32_t Strengths = ((((BegingStrength - 1) & 0x07) << (3 * 0)) | (((EndStrength - 1) & 0x07) << (3 * 1)));
	if (Hand == EControllerHand::Left || Hand == EControllerHand::AnyHand)
	{
		HidOutput->LeftTrigger.Mode = 0x22;
		HidOutput->LeftTrigger.Strengths.ActiveZones = ActiveZones;
		HidOutput->LeftTrigger.Strengths.StrengthZones = Strengths;
	}

	if (Hand == EControllerHand::Right || Hand == EControllerHand::AnyHand)
	{
		HidOutput->RightTrigger.Mode = 0x22;
		HidOutput->RightTrigger.Strengths.ActiveZones = ActiveZones;
		HidOutput->RightTrigger.Strengths.StrengthZones = Strengths;
	}

	SendOut();
}

void UDualSenseLibrary::StopTrigger(const EControllerHand& Hand)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	if (Hand == EControllerHand::Left || Hand == EControllerHand::AnyHand)
	{
		HidOutput->LeftTrigger.Mode = 0x0;
	}

	if (Hand == EControllerHand::Right || Hand == EControllerHand::AnyHand)
	{
		HidOutput->RightTrigger.Mode = 0x0;
	}

	SendOut();
}

void UDualSenseLibrary::StopAll()
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	HidOutput->Feature.VibrationMode = 0xFF;
	HidOutput->Feature.FeatureMode = 0xF7;
	if (
		HidOutput->Lightbar.A == 0 &&
		HidOutput->Lightbar.B == 0 &&
		HidOutput->Lightbar.R == 0
	)
	{
		HidOutput->Lightbar.B = 255;
	}

	if (ControllerID <= 1)
	{
		HidOutput->PlayerLed.Led = static_cast<unsigned char>(ELedPlayerEnum::One);
	}

	if (ControllerID == 2)
	{
		HidOutput->PlayerLed.Led = static_cast<unsigned char>(ELedPlayerEnum::Two);
	}

	if (ControllerID == 3)
	{
		HidOutput->PlayerLed.Led = static_cast<unsigned char>(ELedPlayerEnum::Three);
	}

	if (ControllerID >= 4)
	{
		HidOutput->PlayerLed.Led = static_cast<unsigned char>(ELedPlayerEnum::All);
	}
	SendOut();
}

void UDualSenseLibrary::SetLightbar(FColor Color, float BrithnessTime, float ToggleTime)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	if ((HidOutput->Lightbar.R != Color.R) || (HidOutput->Lightbar.G != Color.G) || (HidOutput->Lightbar.B != Color.B))
	{
		HidOutput->Lightbar.R = Color.R;
		HidOutput->Lightbar.G = Color.G;
		HidOutput->Lightbar.B = Color.B;
		SendOut();
	}
}

void UDualSenseLibrary::SetPlayerLed(ELedPlayerEnum Led, ELedBrightnessEnum Brightness)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	if ((HidOutput->PlayerLed.Led != static_cast<unsigned char>(Led)) || (HidOutput->PlayerLed.Brightness != static_cast
		<unsigned char>(Brightness)))
	{
		HidOutput->PlayerLed.Led = static_cast<unsigned char>(Led);
		HidOutput->PlayerLed.Brightness = static_cast<unsigned char>(Brightness);
		SendOut();
	}
}

void UDualSenseLibrary::SetMicrophoneLed(ELedMicEnum Led)
{
	FOutputContext* HidOutput = &HIDDeviceContexts.Output;
	if (HidOutput->MicLight.Mode != static_cast<unsigned char>(Led))
	{
		HidOutput->MicLight.Mode = static_cast<unsigned char>(Led);
		SendOut();
	}
}

void UDualSenseLibrary::EnableTouch(const bool bIsTouch)
{
	bEnableTouch = bIsTouch;
}

void UDualSenseLibrary::EnableMotionSensor(bool bIsMotionSensor)
{
	bEnableAccelerometerAndGyroscope = bIsMotionSensor;
}

bool UDualSenseLibrary::GetMotionSensorCalibrationStatus(float& OutProgress)
{
	if (!bIsCalibrating)
	{
		OutProgress = 1.0f;
		return false;
	}

	const double ElapsedTime = FPlatformTime::Seconds() - CalibrationStartTime;
	OutProgress = FMath::Clamp(ElapsedTime / CalibrationDuration, 0.0, 1.0);

	if (ElapsedTime >= CalibrationDuration)
	{
		if (CalibrationSampleCount > 0)
		{
			GyroBaseline.X = AccumulatedGyro.X / CalibrationSampleCount;
			GyroBaseline.Y = AccumulatedGyro.Y / CalibrationSampleCount;
			GyroBaseline.Z = AccumulatedGyro.Z / CalibrationSampleCount;

			AccelBaseline.X = AccumulatedAccel.X / CalibrationSampleCount;
			AccelBaseline.Y = AccumulatedAccel.Y / CalibrationSampleCount;
			AccelBaseline.Z = AccumulatedAccel.Z / CalibrationSampleCount;
		}
		bIsCalibrating = false;
		bHasMotionSensorBaseline = true;
		return false;
	}

	return true;
}

void UDualSenseLibrary::StartMotionSensorCalibration(float Duration, float DeadZone)
{
	bIsCalibrating = true;
	CalibrationSampleCount = 0;

	GyroBaseline = FVector::ZeroVector;
	AccelBaseline = FVector::ZeroVector;
	AccumulatedGyro = FVector::ZeroVector;
	AccumulatedAccel = FVector::ZeroVector;

	SensorsDeadZone = FMath::Clamp(DeadZone, 0.0f, 1.f);
	CalibrationDuration = FMath::Clamp(Duration, 1.0f, 10.0f);
	CalibrationStartTime = FPlatformTime::Seconds();
}

void UDualSenseLibrary::SetHasPhoneConnected(const bool HasConnected)
{
	HasPhoneConnected = HasConnected;
}

void UDualSenseLibrary::SetLevelBattery(const float Level, bool FullyCharged, bool Charging)
{
	LevelBattery = Level;
}
