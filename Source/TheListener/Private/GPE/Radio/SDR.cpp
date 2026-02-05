#include "GPE/Radio/SDR.h"

#include "DataWrappers/ChaosVDParticleDataWrapper.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "System/Frequency/FrequencySubsystem.h"

class APlayGameMode;

ASDR::ASDR()
{
	PrimaryActorTick.bCanEverTick = true;

	check(RootComponent);
	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	BaseMeshComponent->SetupAttachment(RootComponent);
	check(BaseMeshComponent);
}

void ASDR::BeginPlay()
{
	Super::BeginPlay();

	Clear();

	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, FName("Refresh"));
	GetWorldTimerManager().SetTimer(RefreshTimer, TimerDel, RefreshRate, true);

	if (!ensure(LFRenderer))
	{
		return;
	}
	LFRendererDynamic = UMaterialInstanceDynamic::Create(LFRenderer, this);
	check(LFRendererDynamic)
	if (!ensure(MFRenderer))
	{
		return;
	}
	MFRendererDynamic = UMaterialInstanceDynamic::Create(MFRenderer, this);
	check(MFRendererDynamic)
	if (!ensure(HFRenderer))
	{
		return;
	}
	HFRendererDynamic = UMaterialInstanceDynamic::Create(LFRenderer, this);
	check(HFRendererDynamic)

	UFrequencySubsystem* FrequencySubsystem = GetWorld()->GetSubsystem<UFrequencySubsystem>();
	if (!FrequencySubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Current Level NOT in subsystem list! Radio will not work"));
		return;
	}

	FrequencySubsystem->RegisterReceiver(LFRendererDynamic);
	FrequencySubsystem->RegisterReceiver(MFRendererDynamic);
	FrequencySubsystem->RegisterReceiver(HFRendererDynamic);
	FrequencySubsystem->RegisterReceiver(this);
}

void ASDR::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(RefreshTimer);
}

void ASDR::Refresh() const
{
	if (!ensure(LFRenderTarget) || !ensure(LFRendererDynamic))
	{
		return;
	}
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), LFRenderTarget, LFRendererDynamic);
	if (!ensure(MFRenderTarget) || !ensure(MFRendererDynamic))
	{
		return;
	}
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), MFRenderTarget, MFRendererDynamic);
	if (!ensure(HFRenderTarget) || !ensure(HFRendererDynamic))
	{
		return;
	}
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), HFRenderTarget, HFRendererDynamic);
}

void ASDR::Clear() const
{
	check(LFRenderTarget)
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), LFRenderTarget, ClearColor);
	check(MFRenderTarget)
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), MFRenderTarget, ClearColor);
	check(HFRenderTarget)
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), HFRenderTarget, ClearColor);
}
