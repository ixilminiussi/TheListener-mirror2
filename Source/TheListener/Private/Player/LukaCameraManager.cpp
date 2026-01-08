#include "Player/LukaCameraManager.h"

#include "Kismet/GameplayStatics.h"
#include "Player/LukaCharacter.h"
#include "Player/LukaController.h"

void ALukaCameraManager::BeginPlay()
{
	Super::BeginPlay();

	ALukaController* LukaController = Cast<ALukaController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	check(LukaController);
	LukaController->OnPossessToy.AddDynamic(this, &ALukaCameraManager::OnPossessToy);
	LukaController->OnUnpossessToy.AddDynamic(this, &ALukaCameraManager::OnUnpossessToy);

	ALukaCharacter* LukaCharacter = LukaController->GetLukaCharacter();
	if (!ensure(LukaCharacter)) return;
	LukaCharacter->OnMove.AddDynamic(this, &ALukaCameraManager::OnMove);
	LukaCharacter->OnStopMoving.AddDynamic(this, &ALukaCameraManager::OnStopMoving);

	CameraShake = Cast<ULegacyCameraShake>(StartCameraShake(CameraShakeClass));
	if (!ensure(CameraShake)) return;
	OnStopMoving(); // Initialize the shake as an idle shake
}

ALukaCameraManager::ALukaCameraManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALukaCameraManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!CameraShake) return;
	CameraShake->RotOscillation.Pitch.Amplitude = FMath::Lerp(CameraShake->RotOscillation.Pitch.Amplitude, TargetShake.Pitch.Amplitude, LerpAlpha);
	CameraShake->RotOscillation.Pitch.Frequency = FMath::Lerp(CameraShake->RotOscillation.Pitch.Frequency, TargetShake.Pitch.Frequency, LerpAlpha);
	CameraShake->RotOscillation.Yaw.Amplitude = FMath::Lerp(CameraShake->RotOscillation.Yaw.Amplitude, TargetShake.Yaw.Amplitude, LerpAlpha);
	CameraShake->RotOscillation.Yaw.Frequency = FMath::Lerp(CameraShake->RotOscillation.Yaw.Frequency, TargetShake.Yaw.Frequency, LerpAlpha);
	CameraShake->RotOscillation.Roll.Amplitude = FMath::Lerp(CameraShake->RotOscillation.Roll.Amplitude, TargetShake.Roll.Amplitude, LerpAlpha);
	CameraShake->RotOscillation.Roll.Frequency = FMath::Lerp(CameraShake->RotOscillation.Roll.Frequency, TargetShake.Roll.Frequency, LerpAlpha);
}

void ALukaCameraManager::OnPossessToy()
{
	TargetShake.Pitch.Amplitude = ToyShake.Pitch.Amplitude;
	TargetShake.Pitch.Frequency = ToyShake.Pitch.Frequency;
	TargetShake.Yaw.Amplitude = ToyShake.Yaw.Amplitude;
	TargetShake.Yaw.Frequency = ToyShake.Yaw.Frequency;
	TargetShake.Roll.Amplitude = ToyShake.Roll.Amplitude;
	TargetShake.Roll.Frequency = ToyShake.Roll.Frequency;
}

void ALukaCameraManager::OnUnpossessToy()
{
	SetTargetShakeAsIdleShake();
}

void ALukaCameraManager::OnMove(float Speed)
{
	TargetShake.Pitch.Amplitude = IdleShake.Pitch.Amplitude + (MaxWalkShake.Pitch.Amplitude - IdleShake.Pitch.Amplitude) * Speed;
	TargetShake.Pitch.Frequency = IdleShake.Pitch.Frequency + (MaxWalkShake.Pitch.Frequency - IdleShake.Pitch.Frequency) * Speed;
	TargetShake.Yaw.Amplitude = IdleShake.Yaw.Amplitude + (MaxWalkShake.Yaw.Amplitude - IdleShake.Yaw.Amplitude) * Speed;
	TargetShake.Yaw.Frequency = IdleShake.Yaw.Frequency + (MaxWalkShake.Yaw.Frequency - IdleShake.Yaw.Frequency) * Speed;
	TargetShake.Roll.Amplitude = IdleShake.Roll.Amplitude + (MaxWalkShake.Roll.Amplitude - IdleShake.Roll.Amplitude) * Speed;
	TargetShake.Roll.Frequency = IdleShake.Roll.Frequency + (MaxWalkShake.Roll.Frequency - IdleShake.Roll.Frequency) * Speed;
}

void ALukaCameraManager::OnStopMoving()
{
	SetTargetShakeAsIdleShake();
}

void ALukaCameraManager::SetTargetShakeAsIdleShake()
{
	TargetShake.Pitch.Amplitude = IdleShake.Pitch.Amplitude;
	TargetShake.Pitch.Frequency = IdleShake.Pitch.Frequency;
	TargetShake.Yaw.Amplitude = IdleShake.Yaw.Amplitude;
	TargetShake.Yaw.Frequency = IdleShake.Yaw.Frequency;
	TargetShake.Roll.Amplitude = IdleShake.Roll.Amplitude;
	TargetShake.Roll.Frequency = IdleShake.Roll.Frequency;
}
