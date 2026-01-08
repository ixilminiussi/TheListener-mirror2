#include "GPE/Radio/SDR.h"

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

	if (!ensure(ScreenMaterial))
	{
		return;
	}
	ScreenMaterialDynamic = UMaterialInstanceDynamic::Create(ScreenMaterial, this);
	check(ScreenMaterialDynamic)

	UFrequencySubsystem* FrequencySubsystem = GetWorld()->GetSubsystem<UFrequencySubsystem>();
	if (!FrequencySubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Current Level NOT in subsystem list! Radio will not work"));
		return;
	}

	FrequencySubsystem->RegisterReceiver(ScreenMaterialDynamic);
}

void ASDR::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(RefreshTimer);
}

void ASDR::Refresh() const
{
	if (!ensure(RenderTarget) || !ensure(ScreenMaterialDynamic))
	{
		return;
	}
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), RenderTarget, ScreenMaterialDynamic);
}

void ASDR::Clear() const
{
	check(RenderTarget)
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), RenderTarget, ClearColor);
}
