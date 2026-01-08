#include "GPE/Radio/ScreenComponent.h"

#include "ConstantQNRT.h"
#include "UI/Dialogue/AnswerDataAsset.h"
#include "GPE/Radio/Radio.h"
#include "GPE/Radio/Station.h"
#include "GPE/Radio/StationDataAsset.h"
#include "Niagara/Public/NiagaraComponent.h"

#define FFT_RANGE 48

AScreenComponent::AScreenComponent()
{
	PrimaryActorTick.bCanEverTick = true;

	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScreenStaticMesh"));
	Plane->SetupAttachment(RootComponent);
	check(Plane);
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(Plane);
	check(NiagaraComponent);

	BackgroundNoiseArray.Init(0.f, FFT_RANGE);
	QNRTAnalysisArray.Init(0.f, FFT_RANGE);
}

void AScreenComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(NoiseTimerHandle, [this]() {this->GenerateNoiseArray();}, NoiseTimerInterval, true);

	StaticNoiseArray.Init(0.f, FFT_RANGE);
	BackgroundNoiseArray.Init(0.f, FFT_RANGE);
	QNRTAnalysisArray.Init(0.f, FFT_RANGE);
}

void AScreenComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(NoiseTimerHandle);
}

void AScreenComponent::GenerateNoiseArray()
{
	if (!ensure(BackgroundNoiseArray.Num() == FFT_RANGE)) // hard coded at constructor, should never change
	{
		return;
	} 
	for (uint32 i = 0; i < FFT_RANGE; i++)
	{
		float RandFloat = FMath::FRandRange(0.0f, BackgroundNoiseStrength);
		BackgroundNoiseArray[i] = RandFloat;
	}
	
	if (!ensure(StaticNoiseArray.Num() == FFT_RANGE)) // hard coded at constructor, should never change
	{
		return;
	} 
	for (uint32 i = 0; i < FFT_RANGE; i++)
	{
		float RandFloat = FMath::FRandRange(0.0f, StaticStrength);
		StaticNoiseArray[i] = RandFloat;
	}
}

void AScreenComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RefreshQNRT();
}

void AScreenComponent::SetRadio(class ARadio* Radio)
{
	Parent = Radio;
}

void AScreenComponent::RefreshQNRT()
{
	check(StaticNoiseArray.Num() > 0);
	check(BackgroundNoiseArray.Num() > 0);
	TArray<float> OutputArray = StaticNoiseArray;

	if (Parent)
	{
		bool bIsInRange;
		AStation const *NearestStation = Parent->GetNearestStation(bIsInRange);
		if (bIsInRange && NearestStation)
		{
			UConstantQNRT const *ConstantQNRT = QNRTInfoDataAsset.Get()->GetConstantQNRT(NearestStation->GetAudioEvent());

			if (ConstantQNRT)
			{
				if (ConstantQNRT->Sound)
				{
					float CurrentPlayingPosition = NearestStation->GetPlayPosition() / 1000.f;
					ConstantQNRT->GetNormalizedChannelConstantQAtTime(CurrentPlayingPosition, 0, QNRTAnalysisArray);

					for (int i = 0; i < OutputArray.Num(); i++)
					{
						OutputArray[i] = FMath::Max(BackgroundNoiseArray[i], FMath::Lerp(StaticNoiseArray[i], QNRTAnalysisArray[i], NearestStation->ComputeRawClarity(Parent->GetFrequency())));
					}
				}
			}
		}
	}
		
	SetNiagaraFloatArray(OutputArray);
}
