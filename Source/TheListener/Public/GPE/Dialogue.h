#pragma once

#include "CoreMinimal.h"
#include "AkGameplayTypes.h"
#include "GameFramework/Actor.h"
#include "Dialogue.generated.h"

#define INVALID_ID -1

DECLARE_DYNAMIC_DELEGATE_OneParam(FDialogueExit, ADialogue*, Dialogue);

UCLASS()
class THELISTENER_API ADialogue : public AActor
{
	GENERATED_BODY()

	// Station ================================
public:
	friend class FCogGameplayWindow_Utilities;

	ADialogue();
	virtual void Destroyed() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void Skip(bool bAll = false);

	UFUNCTION(BlueprintNativeEvent, Category = "Dialogue")
	void Initialize();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetupDialogue(class UDialogueGraphAsset* DialogueAsset, class AToy* Parent);

	UFUNCTION(BlueprintCallable, Category = "Station")
	void Lock(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FDialogueExit &StartDialogue();
	
	friend class FCogGameplayWindow_Utilities;
protected:
	FDialogueExit DialogueExit;
	
	UPROPERTY()
	TObjectPtr<class UDialogueGraphAsset> DialogueAsset;
	
	int32 QuestionID = INVALID_ID; 

	UFUNCTION(BlueprintCallable, Category = "Station")
	void PostAkEvent();
	UFUNCTION()
	void OnAkEventCallback(EAkCallbackType CallbackType, UAkCallbackInfo* CallbackInfo);
	
	AkPlayingID PlayingID = AK_INVALID_PLAYING_ID;
	
	UPROPERTY()
	TObjectPtr<class AToy> Parent = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "Station")
	TObjectPtr<class UAkComponent> AkComponent = nullptr;
	
	UPROPERTY()
	TObjectPtr<class UAkAudioEvent> AudioEvent;

	UPROPERTY(BlueprintReadOnly, Category = "Station")
	bool bIsPlaying = false;
	// Station End ============================

	// Subtitles ==============================
	UFUNCTION(BlueprintCallable, Category = "Station|Subtitles")
	void CreateSubtitles();
	virtual void HandleMarker(FString const& Marker);
	UFUNCTION(BlueprintCallable, Category = "Station|Subtitles")
	void DestroySubtitles();
	
	int32 SubtitlesID = INVALID_ID;
	// Subtitles End ==========================

	// Dialogue ==============================
public:
	class UAkAudioEvent *GetAudioEvent() const;

	bool ProposeAnswer();

	bool bWaitingForPromptCheck = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Station|Prompt")
	FName PromptName = "PRT_RadioAnswer";
	void PlayerQuitRadio();

	
protected:
	bool bEarlyDialogueFlag = false;
	
	void OpenPrompt() const;
	void ClosePrompt();

	virtual bool IsHeard();
	
	UFUNCTION(BlueprintCallable, Category = "Station|Dialogue")
	void AskQuestion();

	void EndDialogue(bool bRepeat = false);
	UFUNCTION(BlueprintNativeEvent, Category = "Station|Dialogue")
	void OnEndDialogue();
	virtual void OnEndDialogue_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Station|Dialogue")
	void OnAnswerQuestion();
	virtual void OnAnswerQuestion_Implementation();

	UFUNCTION()
	void OnAnswerSelected(UAnswer* Answer);
	UFUNCTION()
	virtual void OnFinishDialogue();
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	[[nodiscard]] bool StepNextDialogue();
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StepIntoDialogue(UDialogueLineData* AnswerID);

	UPROPERTY()
	TObjectPtr<class UDialogueLineData> DialogueData;

	AkPlayingID IgnorePlayingID = INVALID_ID;

protected:
	void SetClarity(float Clarity);
	float ClarityThreshold = 0.001f;
	bool bCanHear = false;
};