#include "GPE/Phone.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "AkComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"
#include "GPE/Dialogue.h"
#include "System/Events/EventAction.h"
#include "Miscellaneous/TLUtils.h"
#include "Player/LukaController.h"

APhone::APhone()
{
	PhoneBaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Phone Base");
	check(PhoneBaseMeshComponent);

	PhoneDialMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Phone Dial");
	check(PhoneDialMeshComponent);

	PhoneDialNumbersContainer = CreateDefaultSubobject<USceneComponent>(
		"Phone Dial number Container");
	check(PhoneDialNumbersContainer);

	check(PhoneDialNumbersContainer);


	PhoneBaseMeshComponent->SetupAttachment(CollisionComponent);
	PhoneDialMeshComponent->SetupAttachment(PhoneBaseMeshComponent);
	PhoneDialNumbersContainer->SetupAttachment(PhoneBaseMeshComponent);
	for (int i = 0; i < 10; i++)
	{
		FString NumberName = "Number_";
		NumberName.AppendInt(i);
		UTextRenderComponent* NumberWidget = CreateDefaultSubobject<UTextRenderComponent>(
			FName(*NumberName));
		NumberWidget->SetText(FText::FromString(FString::FromInt(i)));
		NumberWidget->SetupAttachment(PhoneDialNumbersContainer);
		InputNumberTextRenders.Add(NumberWidget);
	}

	for (int i = 0; i < 6; i++)
	{
		FString NumberName = "NumberDigit_";
		NumberName.AppendInt(i);

		UChildActorComponent* ChildActorComponent = CreateDefaultSubobject<
			UChildActorComponent>(FName(*NumberName));
		check(ChildActorComponent);
		ChildActorComponent->SetupAttachment(RootComponent);
		ChildActorComponent->bEditableWhenInherited = true;
		if (NumberDigitClass)
		{
			ChildActorComponent->SetChildActorClass(NumberDigitClass);
		}
		NumberDigits.Push(ChildActorComponent);
	}

	PhoneAkComponent = CreateDefaultSubobject<UAkComponent>(TEXT("PhoneAkComponent"));
	PhoneAkComponent->SetupAttachment(RootComponent);
}

void APhone::BeginPlay()
{
	Super::BeginPlay();

	TArray<USceneComponent*> NumberComponents;
	PhoneDialNumbersContainer->GetChildrenComponents(false, NumberComponents);
	for (int i = 0; i < 10; i++)
	{
		FString NameToCheck = "Number_";
		NameToCheck.AppendInt(i);
		for (USceneComponent* NumberComp : NumberComponents)
		{
			FString NameOfComp;
			NumberComp->GetName(NameOfComp);
			if (NameOfComp == NameToCheck)
			{
				InputNumberTextRenders[i] = Cast<UTextRenderComponent>(NumberComp);
				InputNumberTextRenders[i]->SetTextRenderColor(UnselectedColor);
				break;
			}
		}
	}

	BaseRotation = PhoneDialMeshComponent->GetRelativeRotation();

	ClearDial();
}

void APhone::EndPlay(const EEndPlayReason::Type EndPlayReason) 
{
	Super::EndPlay(EndPlayReason);
	
	while (!CallQueue.IsEmpty())
	{
		auto &[Dialogue, Important, RingTime] = CallQueue.First();
		CallQueue.PopFirst();
		Dialogue->Destroy();
	}
	for (auto &[Number, CallInfo] : UnlockedCalls)
	{
		for (auto &[Dialogue, Time] : CallInfo)
		{
			Dialogue->Destroy();
		}
		CallInfo.Empty();
	}
	UnlockedCalls.Empty();
}

void APhone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (!ensure(EnhancedInputComponent != nullptr)) { return; }
	if (ensure(InputActionRotate != nullptr))
	{
		EnhancedInputComponent->BindAction(InputActionRotate, ETriggerEvent::Triggered, this,
		                                   &APhone::RotateDial);
		EnhancedInputComponent->BindAction(InputActionConfirm, ETriggerEvent::Started, this,
		                                   &APhone::ReleaseDial);
		EnhancedInputComponent->BindAction(InputActionClear, ETriggerEvent::Started, this,
		                                   &APhone::ClearDial);
	}
}

void APhone::PushCall(UDialogueTreeData* DialogueTree, const bool bImportant, const float RingDuration, const bool bSkipQueue)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	
	ADialogue *Dialogue = GetWorld()->SpawnActor<ADialogue>(SpawnParameters);
	Dialogue->SetupDialogue(DialogueTree, this);

	if (bSkipQueue)
	{
		CallQueue.PushFirst({Dialogue, RingDuration, bImportant});
	} else
	{
		CallQueue.PushLast({Dialogue, RingDuration, bImportant});
	}

	if (!bCallMutex && !Cast<ALukaController>(GetController()))
	{
		RingPhone();
	}
}

void APhone::UnlockCall(TArray<int> const& Number, UDialogueTreeData* DialogueTree, float InWaitTime,
						bool bOverrideAll)
{
	if (UnlockedCalls.Contains(Number) && bOverrideAll)
	{
		for (auto &[Dialogue, Time] : UnlockedCalls[Number])
		{
			Dialogue->Destroy();
		}
	}
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;

	ADialogue *Dialogue = GetWorld()->SpawnActor<ADialogue>(SpawnParameters);
	Dialogue->SetupDialogue(DialogueTree, this);

	UnlockedCalls.FindOrAdd(Number).Add({Dialogue, InWaitTime});
}

void APhone::StartCall(ADialogue* Dialogue)
{
	if (!ensure(Dialogue))
	{
		return;
	}
	GetWorldTimerManager().ClearTimer(CallTimerHandle);
	GetWorldTimerManager().ClearTimer(PeopleAnswerTimerHandle);
	AkComponent->PostAkEvent(RingStopEvent);

	Lock(true);
	Dialogue->StartDialogue().BindUFunction(this, FName("EndDialogue"));
}

void APhone::RingPhone()
{
	check(!StandbyDialogue) // make sure we're not already ringing
	
	if (CallQueue.IsEmpty())
	{
		return;
	}

	check(AkComponent)
	AkComponent->PostAkEvent(RingPlayEvent);

	FCallInfo &Info = CallQueue.First();

	GetWorldTimerManager().SetTimer(CallTimerHandle, [this, &Info]()
	{
		EndRing(Info);
	}, Info.CallTime, false);

	StandbyDialogue = Info.Dialogue;
}

void APhone::EndRing(FCallInfo const &Info)
{
	StandbyDialogue = nullptr;
	
	bCallMutex = false;

	check(AkComponent)
	AkComponent->PostAkEvent(RingStopEvent);

	QueueNext();

	if (Info.bImportant)
	{
		CallQueue.PushLast(Info);
	} else
	{
		Info.Dialogue->Destroy();
	}
}

void APhone::EndDialogue(ADialogue *Dialogue)
{
	KickPlayer();

	if (Dialogue)
	{
		bCallMutex = false;
	}

	StandbyDialogue = nullptr; 
}

void APhone::QueueNext()
{
	if (!CallQueue.IsEmpty())
	{
		GetWorld()->GetTimerManager().SetTimer(CallTimerHandle, [this]()
		{
			bCallMutex = true;
			RingPhone();
		}, FMath::RandRange(DelayForNextCall.X, DelayForNextCall.Y), false);
	}
}

void APhone::RotateDial(const FInputActionValue& Value)
{
	const FVector2d RecordedValue = Value.Get<FVector2D>();
	const float Length = RecordedValue.Length();
	float Angle = UTLUtils::GetAngleFrom2DVector(RecordedValue) - StartInputAngle;
	if (Angle < 0)
	{
		Angle += 360;
	}
	int NewNumber = (Angle / AngleDeltaForNumberSelection) + 1;
	if (NewNumber == 10) { NewNumber = 0; }
	if (NewNumber > 10) { NewNumber = -1; } //Invalid Number

	CurrentNumberSelected = NewNumber;
	if (CurrentNumberSelected != LastNumberSelected)
	{
		if (CurrentNumberSelected != -1)
		{
			InputNumberTextRenders[CurrentNumberSelected]->SetTextRenderColor(SelectedColor);
		}
		if (LastNumberSelected != -1)
		{
			InputNumberTextRenders[LastNumberSelected]->SetTextRenderColor(UnselectedColor);
		}
		LastNumberSelected = CurrentNumberSelected;
	}
}

void APhone::ReleaseDial(const FInputActionValue& Value)
{
	if (CurrentNumberSelected == -1) { return; } //Invalidate if current number is invalid
	if (CurrentNumberInput.Num() == 6)
	{
		ClearDial();
	}
	CurrentNumberInput.Push(CurrentNumberSelected);
	OnNumberUpdate_(CurrentNumberInput);
	UE_LOG(LogTemp, Log, TEXT("Selected %i ! Full number is %s"), CurrentNumberSelected,
	       *FString::JoinBy(CurrentNumberInput, TEXT(""), [](int32 Value){ return FString::FromInt(Value); }));
	if (CurrentNumberInput.Num() == 6)
	{
		UE_LOG(LogTemp, Log, TEXT("Calling %s !"),
			   *FString::JoinBy(CurrentNumberInput, TEXT(""), [](int32 Value){ return FString::FromInt(Value); }));

		if (UnlockedCalls.Contains(CurrentNumberInput))
		{
			if (UnlockedCalls[CurrentNumberInput].Num() > 0)
			{
				auto &[Dialogue, Time] = UnlockedCalls[CurrentNumberInput][0];
				UnlockedCalls[CurrentNumberInput].RemoveAt(0);
				
				Lock(true);
				AkComponent->PostAkEvent(RingPlayEvent);

				GetWorldTimerManager().SetTimer(PeopleAnswerTimerHandle, [this, Dialogue]()
				{
					StartCall(Dialogue);
				}, Time, false);
			}
		} else
		{
		}
	}
}

void APhone::ClearDial()
{
	CurrentNumberInput.Reset();
	OnNumberUpdate_(CurrentNumberInput);
}

void APhone::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (ALukaController *LukaController = Cast<ALukaController>(NewController); LukaController == nullptr)
	{
		return;
	}
	
	if (StandbyDialogue != nullptr)
	{
		StartCall(StandbyDialogue);
	}
	else
	{
		ClearDial();
	}
}

void APhone::UnPossessed()
{
	if (CurrentNumberSelected != -1)
	{
		InputNumberTextRenders[CurrentNumberSelected]->SetTextRenderColor(SelectedColor);
		ClearDial();
	}
	QueueNext();
	Super::UnPossessed();
}
