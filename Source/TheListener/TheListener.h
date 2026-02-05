#pragma once

#include "CoreMinimal.h"

#define LOG(x, ...) UE_LOG(LogTemp, Log, TEXT(x), __VA_ARGS__)
#define LOG_NAKED(x) UE_LOG(LogTemp, Log, TEXT(x))
#define LOG_WARNING(x, ...) UE_LOG(LogTemp, Warning, TEXT(x), __VA_ARGS__)
#define LOG_ERROR(x, ...) UE_LOG(LogTemp, Error, TEXT(x), __VA_ARGS__)