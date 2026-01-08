#pragma once

#include "CoreMinimal.h"
#include "GPE/Toy.h"
#include "Radio.generated.h"

/* DEPRECATED
UENUM()
enum class EFrequencyControlType : uint8
{
	EFCT_Spin = 0 UMETA(DisplayName = "Spin"),
	EFCT_Shift UMETA(DisplayName = "Shift")
};
*/

UCLASS(Blueprintable)
class THELISTENER_API ARadio : public AToy
{
	GENERATED_BODY()

	friend class FCogGameplayWindow_Utilities;

protected:
	ARadio();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;;
	virtual void Destroyed() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	void StartRadio();
	void StopRadio();

	// Stations Begin =========================
public:
	UFUNCTION(BlueprintCallable)
	float GetRelativeStationPosition(AStation *Station);
	
	class AStation* CreateStationObject(class UStationAsset* StationData);
	class AStation* GetStationFromDataAsset(const class UStationAsset* StationData);
	void ForgetStationObject(class AStation* Station);

protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<class AStation>> Stations{};
	// Stations End ===========================

	// Sound Begin ============================
public:
	void UpdateStations();
	TSet<TObjectPtr<class AStation>> const &GetStationsInRange() const;
	UFUNCTION(BlueprintCallable, Category = "Toy|Radio|Stations")
	class AStation *GetNearestStation(bool &bIsInRange) const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Toy|Radio")
	void OnClarityUpdate(float InClarity);
	
	TSet<TObjectPtr<class AStation>> StationsInRange;

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio|Stations")
	TObjectPtr<class USceneComponent> SoundPosition = nullptr;

	UPROPERTY()
	TObjectPtr<class UAkComponent> RadioAkComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio")
	TObjectPtr<class UAkAudioEvent> NoiseStartEvent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio")
	TObjectPtr<class UAkAudioEvent> NoiseStopEvent = nullptr;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio")
	TObjectPtr<class UAkRtpc> NoiseRtpc = nullptr;
	// Sound End ==============================

	// Inputs Begin ===========================
	void Lock(bool bEnable, AStation* Station = nullptr);

protected:
	FTimerHandle LockTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio")
	float LerpSpeed = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionShiftFrequency = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Enhanced Input")
	TObjectPtr<class UInputAction> InputActionAnswer = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Toy|Enhanced Input")
	class UInputMappingContext* DeactivateRadioInputMappingContext;

	// Triggers
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio|Controls|Triggers",
		meta = (ClampMin = "0.1", ClampMax = "1000.0"))
	float Sensitivity = 800.f;
	// Inputs End =============================

	// Frequency Begin ========================
public:
	FVector2d GetFrequencyRange() const;
	UFUNCTION(BlueprintCallable, Category = "Toy|Radio")
	float GetFrequency() const;

protected:
	void ShiftFrequency(const struct FInputActionValue& Value);
	void HandleAnswer(const struct FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio", meta = (ClampMin = "0.0", ClampMax = "20000.0"))
	float StartFrequency = 10000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio", meta = (ClampMin = "0.0", ClampMax = "20000.0"))
	FVector2D FrequencyRange{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Toy|Radio")
	TObjectPtr<class UKnobComponent> KnobComponent{nullptr};
	// Frequency End ==========================

	// Decoder Begin ========================
public:
	class ADecoder *GetDecoder() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Toy|Radio|Decoder")
	TObjectPtr<class UChildActorComponent> DecoderComponent{nullptr};
	UPROPERTY()
	TObjectPtr<class ADecoder> Decoder{nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio|Decoder")
	TSubclassOf<class ADecoder> DecoderComponentClass;

	bool bWasTrue = false;
	// Decoder End ==========================

	// Assets Begin ===========================
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio")
	TObjectPtr<class UTextRenderComponent> TextRenderComponent{nullptr};
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio")
	TObjectPtr<class UWidgetComponent> WidgetComponent{nullptr};
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio")
	TSubclassOf<class URadioText> TextWidgetClass{nullptr};
	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio")
	TObjectPtr<class UStaticMeshComponent> BaseMeshComponent{nullptr};

	void UpdateVisuals() const;
	void UpdateText() const;
	// Assets End =============================

	// Subtitles Begin ========================
	UPROPERTY(Category = "Toy|Radio", EditDefaultsOnly)
	TObjectPtr<UCurveFloat> SubtitlesCorruptionCurve;
	UPROPERTY(Category = "Toy|Radio", EditDefaultsOnly)
	TObjectPtr<UCurveFloat> AudioCorruptionCurve;
	// Subtitles End ==========================

	// Screen Begin ========================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Toy|Radio|Screen")
	TObjectPtr<class UChildActorComponent> ScreenComponent{nullptr};
	UPROPERTY()
	TObjectPtr<class AScreenComponent> Screen{nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Toy|Radio|Screen")
	TSubclassOf<class AScreenComponent> ScreenComponentClass;
	// Screen End ==========================
};
