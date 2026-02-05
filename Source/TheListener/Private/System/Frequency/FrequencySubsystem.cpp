#include "System/Frequency/FrequencySubsystem.h"

#include "AkAudioEvent.h"
#include "Curves/CurveLinearColor.h"
#include "Curves/CurveLinearColorAtlas.h"
#include "GPE/Radio/Radio.h"
#include "GPE/Radio/SDR.h"
#include "GPE/Radio/Station.h"
#include "GPE/Radio/StationDataAsset.h"
#include "GPE/Radio/StationListDataAsset.h"
#include "Miscellaneous/TLUtils.h"
#include "System/Core/ListenerWorldSettings.h"
#include "System/Frequency/FrequencySubsystemData.h"

bool UFrequencySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const auto World = Cast<UWorld>(Outer);
	if (!World)
	{
		return false;
	}
	if (World->IsGameWorld())
	{
		if (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE)
		{
			const AListenerWorldSettings* WorldSettings = Cast<AListenerWorldSettings>(World->GetWorldSettings());
			if (!WorldSettings)
			{
				return false;
			}

			const auto AllowedLevels = WorldSettings->AllowedLevels;

			for (const auto& Level : AllowedLevels)
			{
				if (Level.IsValid() && !Level.ToSoftObjectPath().IsValid())
				{
					continue;
				}

				if (World->GetName() == Level.GetAssetName())
				{
					return true;
				}
			}
		}
	}
	return false;
}

void UFrequencySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AllowedBands = {0, 1, 2};

	auto World = GetWorld();
	if (!World || !World->IsGameWorld())
	{
		return;
	}
	AListenerWorldSettings* WorldSettings = Cast<AListenerWorldSettings>(World->GetWorldSettings());
	if (!WorldSettings)
	{
		return;
	}

	if (UFrequencySubsystemData* FrequencySubsystemData = WorldSettings->FrequencySubsystemData.LoadSynchronous())
	{
		ReceptionBands.Emplace(FrequencySubsystemData->LfRange);
		ReceptionBands.Emplace(FrequencySubsystemData->MfRange);
		ReceptionBands.Emplace(FrequencySubsystemData->HfRange);

		FrequencyColorCurve = FrequencySubsystemData->FrequencyColorCurve;
		CurveAtlas = FrequencySubsystemData->CurveAtlas;
		
		this->StartingStationList = FrequencySubsystemData->StartingStationList;

		ClearAndZeroCurve(FrequencyColorCurve->FloatCurves);
	}

	FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UFrequencySubsystem::OnActorsInitialized);

	UE_LOG(LogTemp, Log, TEXT("UFrequencySubsystem Initialized"));
}

void UFrequencySubsystem::OnActorsInitialized(const FActorsInitializedParams& Params)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UFrequencySubsystem::InitEvents));
}

void UFrequencySubsystem::InitEvents()
{
	if (StartingStationList)
	{
		for (UStationAsset* StationDataAsset : StartingStationList->GetStationsData())
		{
			AddStation(StationDataAsset);
		}
	}
}

void UFrequencySubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UFrequencySubsystem::RegisterReceiver(UMaterialInstanceDynamic* Material)
{
	ReceiverRegister.Push(Material);
}

void UFrequencySubsystem::RegisterReceiver(class ARadio* Radio)
{
	check(Radio);
	Radios.Add(Radio) = {-1.f, -1.f, -1.f};

	for (UStationAsset *StationAsset : StationDataAssets) 
	{
		Radio->CreateStationObject(StationAsset);
	}

	check(ReceptionBands.Num() > static_cast<int>(Band))
	Radio->SetFrequencyRange(ReceptionBands[Band], -1.f);
}

void UFrequencySubsystem::RegisterReceiver(class ASDR* SDR)
{
	check(SDR);
	SDRs.Push(SDR);

	SDR->SetBand(Band);
}

void UFrequencySubsystem::RefreshCurveAtlas() const
{
	if (!ensure(CurveAtlas) || !ensure(FrequencyColorCurve))
	{
		return;
	}

	ClearAndZeroCurve(FrequencyColorCurve->FloatCurves);
	for (UStationAsset* StationDataAsset : StationDataAssets)
	{
		AddFrequencyToCurve(FrequencyColorCurve->FloatCurves, StationDataAsset);
	}
	CurveFlush(CurveAtlas);
}

void UFrequencySubsystem::AddStation(UStationAsset* StationData)
{
	if (!ensureAlways(StationData))
	{
		return;
	}
	if (StationDataAssets.Contains(StationData))
	{
		return;
	}
	if (!ensure(FrequencyColorCurve) || !ensure(CurveAtlas))
	{
		return;
	}
	for (auto const &[Radio, Freq] : Radios)
	{
		if (ensure(Radio))
		{
			Radio->CreateStationObject(StationData);
		}
	}

	StationDataAssets.Emplace(StationData);
	AddFrequencyToCurve(FrequencyColorCurve->FloatCurves, StationData);
	CurveFlush(CurveAtlas);
}

void UFrequencySubsystem::DestroyStation(const UStationAsset* StationData) const
{
	check(StationData);

	for (auto const &[Radio, Freq] : Radios)
	{
		if (!ensure(Radio)) { return; }

		AStation* Station = Radio->GetStationFromDataAsset(StationData);
		if (ensure(Station))
		{
			Station->Destroy();
		}
	}
}

void UFrequencySubsystem::ForgetStation(AStation* Station)
{
	check(Station);

	for (auto const &[Radio, Freq] : Radios)
	{
		if (ensure(Radio))
		{
			Radio->ForgetStationObject(Station);
		}
	}

	if (!ensure(FrequencyColorCurve) || !ensure(CurveAtlas))
	{
		return;
	}

	StationDataAssets.Remove(Station->GetStationData());
	RemoveFrequencyFomCurve(FrequencyColorCurve->FloatCurves, Station->GetStationData());
	CurveFlush(CurveAtlas);
}

bool UFrequencySubsystem::IsFrequencyAllowed(float Value) const
{
	for (const int Bandino : AllowedBands)
	{
		if (Value >= ReceptionBands[Bandino].X && Value <= ReceptionBands[Bandino].Y)
		{
			return true;
		}
	}

	return false;
}

bool UFrequencySubsystem::NextBand()
{
	int ID = AllowedBands.Find(Band);
	
	if (ID < AllowedBands.Num() - 1)
	{
		SetBand(AllowedBands[ID + 1]);
		return true;
	}
	return false;
}

bool UFrequencySubsystem::LastBand()
{
	int ID = AllowedBands.Find(Band);
	
	if (ID > 0)
	{
		SetBand(AllowedBands[ID - 1]);
		return true;
	}
	return false;
}

void UFrequencySubsystem::SetAllowedBands(TArray<int> InAllowedBands)
{
	check(InAllowedBands.Num() <= 3 && InAllowedBands.Num() > 0) // TODO: NO SUPPORT FOR 0 BANDS YET
	
	for (int i : InAllowedBands)
	{
		if (!ensure(i == 0 || i == 1 || i == 2)) // INVALID BANDS, MUST BE EITHER 0, 1, 2
		{
			return;
		}
	}
	
	AllowedBands = InAllowedBands;
	AllowedBands.Sort();

	if (!AllowedBands.Contains(Band))
	{
		SetBand(AllowedBands[0]);
	}
}

void UFrequencySubsystem::AllowBand(int InBand)
{
	AllowedBands.Add(InBand);
	SetBand(InBand);
}

void UFrequencySubsystem::SetBandFromFrequency(float Frequency)
{
	for (int Bandino : AllowedBands)
	{
		if (Frequency >= ReceptionBands[Bandino].X && Frequency <= ReceptionBands[Bandino].Y)
		{
			SetBand(Bandino);
			return;
		}
	}

	check(false) // very not good
}

void UFrequencySubsystem::SetBand(const uint32 InBand)
{
	for (auto &[Radio, Freq] : Radios)
	{
		Freq[Band] = Radio->GetFrequency();
	}

	Band = InBand;

	check(ReceptionBands.Num() > static_cast<int>(Band));
	
	for (auto const &[Radio, Freq] : Radios)
	{
		Radio->SetFrequencyRange(ReceptionBands[Band], Freq[Band]);
	}
	for (ASDR *SDR : SDRs)
	{
		SDR->SetBand(InBand);
	}
}

void UFrequencySubsystem::ClearAndZeroCurve(FRichCurve* Curve)
{
	if (!ensure(Curve))
	{
		return;
	}

	for (int32 ChannelIndex = 0; ChannelIndex < 3; ++ChannelIndex)
	{
		FRichCurve& RichCurve = Curve[ChannelIndex];
		RichCurve.Reset();
		RichCurve.UpdateOrAddKey(0.0f, 0.0f);
	}
}

void UFrequencySubsystem::CurveFlush(UCurveLinearColorAtlas* Atlas) const
{
	const int32 Width = Atlas->TextureSize;
	const int32 Height = Atlas->GradientCurves.Num();

	UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	NewTexture->Filter = TF_Nearest;
	NewTexture->CompressionSettings = TC_VectorDisplacementmap;
	NewTexture->SRGB = false;
	NewTexture->UpdateResource();

	FTexture2DMipMap& Mip = NewTexture->GetPlatformData()->Mips[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);

	uint8* PixelData = static_cast<uint8*>(Data);

	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			const int32 Index = ((Y * Width) + X) * 4;

			const float Time = static_cast<float>(X) / (Width - 1);

			if (Atlas->GradientCurves.IsValidIndex(Y))
			{
				const FLinearColor Value = Atlas->GradientCurves[Y]->GetLinearColorValue(Time);
				PixelData[Index + 0] = FMath::Clamp(FMath::RoundToInt(Value.B * 255.0f), 0, 255);
				PixelData[Index + 1] = FMath::Clamp(FMath::RoundToInt(Value.G * 255.0f), 0, 255);
				PixelData[Index + 2] = FMath::Clamp(FMath::RoundToInt(Value.R * 255.0f), 0, 255);
				PixelData[Index + 3] = FMath::Clamp(FMath::RoundToInt(Value.A * 255.0f), 0, 255);
			}
		}
	}

	Mip.BulkData.Unlock();
	NewTexture->UpdateResource();

	for (UMaterialInstanceDynamic* MID : ReceiverRegister)
	{
		MID->SetScalarParameterValue("P_FreqTexY", 1.0 / static_cast<float>(Height));
		MID->SetTextureParameterValue("P_FreqAtlas", NewTexture);
	}
}

void UFrequencySubsystem::AddFrequencyToCurve(FRichCurve* Curve, const UStationAsset* StationData) const
{
	if (!ensure(Curve))
	{
		return;
	}

	const float Frequency = StationData->Frequency;
	const float ReceptionBand = StationData->ReceptionBand / 2.;

	const float BeginRange = UTLUtils::FrequencyStep(Frequency, -ReceptionBand);
	const float EndRange = UTLUtils::FrequencyStep(Frequency, ReceptionBand);

	for (int i = 0; i < ReceptionBands.Num(); i++)
	{
		const float Min = ReceptionBands[i].X;
		const float Max = ReceptionBands[i].Y;
		if (Frequency > Min && Frequency < Max)
		{
			const FVector2D LogRange = {FMath::Log2(Min), FMath::Log2(Max)};
	
			auto NormalizeLog = [&LogRange, &i](const float Value) -> float
			{
				const float LogValue = FMath::Log2(Value);

				float Norm = (LogValue - LogRange.X) / (LogRange.Y - LogRange.X);
				return Norm / 3.f + (i / 3.f);
			};	

			Curve->AddKey(NormalizeLog(BeginRange), 0.0f, false, StationData->CurveKeys[0]);
			Curve->AddKey(NormalizeLog(Frequency), 1.0f, false, StationData->CurveKeys[1]);
			Curve->AddKey(NormalizeLog(EndRange), 0.0f, false, StationData->CurveKeys[2]);

			break;
		}
	}
}

void UFrequencySubsystem::RemoveFrequencyFomCurve(FRichCurve* Curve, const UStationAsset* StationData) const
{
	if (!ensure(Curve) || !ensure(StationData))
	{
		return;
	}

	for (auto &Key : StationData->CurveKeys)
	{
		if (Curve->IsKeyHandleValid(Key))
		{
			Curve->DeleteKey(Key);
		}
	}
}
