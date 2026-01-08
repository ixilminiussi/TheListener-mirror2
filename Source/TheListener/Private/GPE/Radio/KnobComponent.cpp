#include "GPE/Radio/KnobComponent.h"
#include "AkComponent.h"

UKnobComponent::UKnobComponent(const FObjectInitializer& ObjectInitializer): ValueSensitivity(0),
                                                                             Flip(false),
                                                                             Value(0),
                                                                             Range(TNumericLimits<float>::Min(),
	                                                                             TNumericLimits<float>::Max()),
                                                                             ClickAngle(0),
                                                                             ClickAkComponent(nullptr),
                                                                             StartClickAudioEvent(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UKnobComponent::ClockwiseRotate(float Amount, const float DeltaTime)
{
	if (SensitivityCurve)
	{
		Amount = (Amount < 0)
			         ? -SensitivityCurve->GetFloatValue(FMath::Abs(Amount))
			         : SensitivityCurve->GetFloatValue(FMath::Abs(Amount));
	}

	Amount *= RotationSensitivity * DeltaTime;

	float ValueAmount = Amount * ValueSensitivity;
	ValueAmount = FMath::Min(Range.Y - Value, ValueAmount);
	ValueAmount = FMath::Max(Range.X - Value, ValueAmount);
	if (FMath::Abs(ValueAmount) <= 0.0001f)
	{
		return;
	}

	Amount = ValueAmount / ValueSensitivity;

	FRotator AddRotator{};
	AddRotator.Roll = Flip ? -Amount : Amount;
	AddRelativeRotation(AddRotator);

	Value += ValueAmount;
	ApplyClamp();
	ClickTest();
}

void UKnobComponent::SetRange(const FVector2D& NewRange)
{
	Range = NewRange;

	if (Range.X > Range.Y)
	{
		Swap(Range.X, Range.Y);
	}

	ApplyClamp();
}

void UKnobComponent::SetValue(const float NewValue)
{
	Value = NewValue;
	ApplyClamp();
}

float UKnobComponent::GetValue() const
{
	return Value;
}

void UKnobComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ClickAngle > 0.f)
	{
		ClickAkComponent = NewObject<UAkComponent>(this);

		check(ClickAkComponent);

		ClickAkComponent->RegisterComponent();
		ClickAkComponent->AttachToComponent(GetAttachmentRoot(), FAttachmentTransformRules::KeepRelativeTransform);
		ClickAkComponent->SetRelativeLocation(FVector::ZeroVector);
	}
}

void UKnobComponent::ApplyClamp()
{
	Value = FMath::Clamp(Value, Range.X, Range.Y);
}

void UKnobComponent::ClickTest() const
{
	if (ClickAngle <= 0.f)
	{
		return;
	}

	static float LastClickRoll = GetComponentRotation().Roll;
	const float CurrentClickRoll = GetComponentRotation().Roll;

	const float ClickRollDifference = CurrentClickRoll - LastClickRoll;
	if (FMath::Abs(ClickRollDifference) >= ClickAngle)
	{
		LastClickRoll = CurrentClickRoll;

		if (StartClickAudioEvent != nullptr)
		{
			ClickAkComponent->PostAkEvent(StartClickAudioEvent);
		}

		OnClick.Broadcast(ClickRollDifference > 0);
	}
}
