#include "GPE/Radio/RadioScreenComponent_Legacy.h"

#include "Curves/RichCurve.h"
#include "GPE/Radio/Station.h"
#include "System/Frequency/FrequencySubsystem.h"

URadioScreenComponent_Legacy::URadioScreenComponent_Legacy(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer),
                                                                                           ReceiverFrequency(0),
                                                                                           GlobalRange(0)
{
	PrimaryComponentTick.bCanEverTick = true;

	SpectrogramStaticMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(
		this, "SpectrogramStaticMesh");
	check(SpectrogramStaticMesh);

	ScreenStaticMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(
		this, "ScreenStaticMesh");
	check(ScreenStaticMesh);
}

void URadioScreenComponent_Legacy::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(SpectrogramMaterialInterface) || !ensure(SpectrogramStaticMesh))
	{
		return;
	}
	SpectrogramMaterialDynamic = UMaterialInstanceDynamic::Create(SpectrogramMaterialInterface, this);
	ScreenStaticMesh->SetMaterial(0, SpectrogramMaterialDynamic);
	SpectrogramMaterialDynamic->SetScalarParameterValue("FrequencyRange", LocalRange);

	const auto FrequencySubsystem = GetWorld()->GetSubsystem<UFrequencySubsystem>();

	if (!FrequencySubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Level Missing in Game Settings, Radio will not work!"));
		return;
	}
	FrequencySubsystem->RegisterReceiver(SpectrogramMaterialDynamic);
}

void URadioScreenComponent_Legacy::SetupAttachments(USceneComponent* SceneComponent) const
{
	check(SpectrogramStaticMesh);
	check(ScreenStaticMesh);

	SpectrogramStaticMesh->SetupAttachment(SceneComponent);
	ScreenStaticMesh->SetupAttachment(SpectrogramStaticMesh);
}

void URadioScreenComponent_Legacy::SetReceiverFrequency(const float Frequency)
{
	ReceiverFrequency = Frequency;

	if (!ensure(SpectrogramMaterialDynamic))
	{
		return;
	}
	SpectrogramMaterialDynamic->SetScalarParameterValue("CurrentFrequency", ReceiverFrequency);
}

void URadioScreenComponent_Legacy::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!ensure(SpectrogramMaterialDynamic))
	{
	}
}