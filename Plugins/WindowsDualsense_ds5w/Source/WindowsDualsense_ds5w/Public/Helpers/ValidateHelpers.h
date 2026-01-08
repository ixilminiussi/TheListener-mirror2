// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
/**
 * @class FValidateHelpers
 * @brief A helper class for validating input values against predefined constraints.
 *
 * The FValidateHelpers class provides static methods to validate various input parameters
 * such as force, position, and frequency. These validations ensure that specific values
 * remain within acceptable ranges for DualSense-related operations. All methods return
 * a boolean indicating whether the value is within the valid range.
 */
class WINDOWSDUALSENSE_DS5W_API FValidateHelpers
{
	/**
	 * Validates whether the given strength value is within the allowable range.
	 *
	 * @param Strength The strength value to validate.
	 * @return True if the strength value is between 0 and 8 (inclusive); otherwise, false.
	 */
public:
	static bool ValidateMaxForce(const int32 Strength);

	/**
	 * Validates whether the given position value is within the allowed range of 0 to 8 (inclusive).
	 *
	 * @param Position The position value to validate.
	 * @return True if the position is within the valid range, false otherwise.
	 */
	static bool ValidateMaxPosition(const int32 Position);

	/**
	 * Validates if the given frequency value is within the allowable range.
	 *
	 * @param Frequency The frequency to be validated. Expected range is between 0.0 and 1.0 inclusive.
	 * @return Returns true if the frequency is within the valid range; otherwise returns false.
	 */
	static bool ValidateMaxFrequency(const float Frequency);

	/**
	 * Converts a normalized float value (ranging from 0.0 to 1.0) to an integer value in the range [0, 255].
	 * Ensures the output is clamped between 0 and 255.
	 *
	 * @param Value The normalized float value to be converted, where 0.0 represents the minimum and 1.0 represents the maximum.
	 * @return An integer ranging from 0 to 255, corresponding to the normalized input value.
	 */
	static int To255(const float Value);

	/**
	 * @brief Converts an unsigned char value to a corresponding value in the range of [0, 255] based on the given maximum input.
	 *
	 * This function scales an input value proportionally within a range of 0 to 255, ensuring that values below 0 return 0
	 * and values exceeding or equal to MaxInput return 255.
	 *
	 * @param Value The input value to be converted, ranging from 0 up to MaxInput.
	 * @param MaxInput The maximum possible input value, which serves as the upper bound for scaling.
	 * @return An integer value in the range [0, 255] corresponding to the scaled input.
	 */
	static int To255(const unsigned char Value, const unsigned char MaxInput);

	/**
	 * Converts the contents of a buffer into a hexadecimal string representation and logs it.
	 *
	 * @param Buffer Pointer to the buffer containing the data to be converted to hexadecimal format.
	 * @param BufferSize The size of the buffer, in bytes.
	 */
	static void PrintBufferAsHex(const unsigned char* Buffer, int BufferSize, const FString& Device = TEXT("Unknow"))
	{
		FString HexString;
		for (int i = 0; i < BufferSize; i++)
		{
			HexString += FString::Printf(TEXT("%02X "), Buffer[i]);
		}
		
		UE_LOG(LogTemp, Log, TEXT("Buffer Device: %s String: %s"), *Device, *HexString);
	}
};
