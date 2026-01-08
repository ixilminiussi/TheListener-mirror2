// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Deque.h"
#include "GPE/Toy.h"
#include "Phone.generated.h"

UCLASS()
class THELISTENER_API APhone : public AToy
{
	GENERATED_BODY()

public:
	APhone();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Phone")
	void OnNumberUpdate_(const TArray<int>& NumberInput);
	
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone|Mesh")
	TObjectPtr<class UStaticMeshComponent> PhoneBaseMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone|Mesh")
	TObjectPtr<class UStaticMeshComponent> PhoneDialMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone")
	TObjectPtr<class USceneComponent> PhoneDialNumbersContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone")
	TArray<TObjectPtr<class UTextRenderComponent>> InputNumberTextRenders;
	//Component End

	//Call Begin
public:
	void PushCall(class UDialogueTreeData* DialogueTree, bool bImportant, float RingDuration, bool bSkipQueue);
	void UnlockCall(TArray<int> const &Number, class UDialogueTreeData *DialogueTree, float InWaitTime, bool bOverrideAll);
	
protected:
	struct FCallInfo
	{
		class ADialogue *Dialogue;
		float CallTime;
		bool bImportant;
	};

	void StartCall(ADialogue *Dialogue);
	void RingPhone();
	void EndRing(FCallInfo const &Info);
	UFUNCTION()
	void EndDialogue(class ADialogue *Dialogue);

	void QueueNext();
	
	bool bCallMutex;
	
	UPROPERTY()
	TObjectPtr<class ADialogue> StandbyDialogue;
	
	TDeque<FCallInfo> CallQueue;
	TMap<TArray<int>, TArray<TPair<TObjectPtr<ADialogue>, float>>> UnlockedCalls; 
	
	FTimerHandle CallTimerHandle;
	FTimerHandle PeopleAnswerTimerHandle;

	UPROPERTY(Category = "Toy|Phone", EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0"))
	FVector2D DelayForNextCall{2, 10};
	//Call End

	//Audio Begin
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Toy|Phone|Audio")
	TObjectPtr<class UAkComponent> PhoneAkComponent;

	UPROPERTY(Category = "Toy|Phone|Audio", EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UAkAudioEvent> RingPlayEvent;
	UPROPERTY(Category = "Toy|Phone|Audio", EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UAkAudioEvent> RingStopEvent;
	// Audio End

	//Input Begin
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionRotate = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionConfirm = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionClear = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone|Controls|Joystick", meta = (ClampMin = "0.01", ClampMax = "1.0"))
	float MinimumRollInputRegister = .6f;

	void RotateDial(const struct FInputActionValue& Value);
	void ReleaseDial(const struct FInputActionValue& Value);
	void ClearDial();
	//Input End

	//Digits Begin
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone")
	TSubclassOf<class AActor> NumberDigitClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Toy|Phone")
	TArray<TObjectPtr<UChildActorComponent>> NumberDigits;
	//Digits End

	//Dial Begin
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone|Dial", meta = (ClampMin = "5.0", ClampMax = "720.0"))
	float DialRewindSpeed = 270.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone|Dial", meta = (ClampMin = "5.0", ClampMax = "720.0"))
	float DialWindSpeed = 180.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone|Dial", meta = (ClampMin = "5.0", ClampMax = "360.0"))
	float MaxDialAngle = 180.0f;

	float CurrentAngle = .0f;
	FRotator BaseRotation;

	TArray<int> CurrentNumberInput{};

	int LastNumberSelected = 0;
	int CurrentNumberSelected = -1;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone|Dial")
	float StartInputAngle = 30;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone|Dial")
	int AngleDeltaForNumberSelection = 30; //(360/12 Holes (theoretically 12 Holes)) 

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone|Dial")
	FColor SelectedColor;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Phone|Dial")
	FColor UnselectedColor;
	//Dial End
};
