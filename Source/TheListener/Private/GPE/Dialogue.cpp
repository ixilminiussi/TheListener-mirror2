#include "GPE/Dialogue.h"

#include "AkComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Miscellaneous/TLUtils.h"
#include "GPE/Toy.h"
#include "UI/Dialogue/AnswerDataAsset.h"
#include "UI/LukaHUD.h"
#include "UI/Subtitles/SubtitlesWidget.h"
#include "UI/Dialogue/AnswerWidget.h"

ADialogue::ADialogue()
{
	PrimaryActorTick.bCanEverTick = false;

	AkComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AkComponent"));
	AkComponent->SetupAttachment(RootComponent);
}

void ADialogue::Destroyed()
{
	Super::Destroyed();

	if (SubtitlesID != INVALID_ID)
	{
		DestroySubtitles();
	}

	if (AkComponent)
	{
		AkComponent->Stop();
	}
}

void ADialogue::BeginPlay()
{
	Super::BeginPlay();

	CreateSubtitles();
}

void ADialogue::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (SubtitlesID != INVALID_ID)
	{
		DestroySubtitles();
	}
	
	if (AkComponent)
	{
		AkComponent->Stop();
	}
}

void ADialogue::Skip(bool bAll)
{
	if (!DialogueTree || bAll) // LEGACY / DEFAULT mode
	{
		EndDialogue();
	} else // DIALOGUE mode
	{
		OnFinishDialogue();
	}
}

void ADialogue::Initialize_Implementation()
{
}

FDialogueExit &ADialogue::StartDialogue()
{
	check(AkComponent);
	check(DialogueTree);

	if (DialogueTree) // Use Answer Runtime
	{
		check(DialogueTree->Nodes.Num() > DialogueTree->Root);
		DialogueData = DialogueTree->Nodes[DialogueTree->Root];
		check(DialogueData); // TODO: BROKEN DIALOGUE TREE DATA
		AudioEvent = DialogueData->GetStartAudioEvent();

		if (!AudioEvent)
		{
			OnFinishDialogue();
		} else
		{
			PostAkEvent();
		}
	}

	return DialogueExit;
}

void ADialogue::PostAkEvent()
{
	if (!ensure(AudioEvent))
	{
		return;
	}
	
	bIsPlaying = true;

	const FName CBName("OnAkEventCallback");
	FOnAkPostEventCallback AkPostEventCallback;
	AkPostEventCallback.BindUFunction(this, CBName);
	PlayingID = AkComponent->PostAkEvent(AudioEvent,
	                                     AK_Marker | AK_MusicSyncUserCue | AK_EndOfEvent | AK_EnableGetMusicPlayPosition
	                                     | AK_EnableGetSourcePlayPosition, AkPostEventCallback);

	check(PlayingID != AK_INVALID_PLAYING_ID);
	/* alors gars, en faite, le son que t'as mis sur ta station il est pas valid
	 * donc soit tu l'as pas mis du tous, soit ta oublie de regenerer tes soundbanks
	 * c'est pas complique putain
	 * fait des efforts */
}

void ADialogue::OnAkEventCallback(EAkCallbackType CallbackType, UAkCallbackInfo* CallbackInfo)
{
	if (Cast<UAkEventCallbackInfo>(CallbackInfo)->PlayingID == IgnorePlayingID)
	{
		return;
	}
	
	switch (CallbackType)
	{
	case EAkCallbackType::Marker:
			HandleMarker(FString(Cast<UAkMarkerCallbackInfo>(CallbackInfo)->Label));
		break;
	case EAkCallbackType::MusicSyncUserCue:
			HandleMarker(FString(Cast<UAkMusicSyncCallbackInfo>(CallbackInfo)->UserCueName));
		break;
	case EAkCallbackType::EndOfEvent:
		if (!bEarlyDialogueFlag)
		{
			OnFinishDialogue();
		} else
		{
			bEarlyDialogueFlag = false;
		}
		break;
	default:
		break;
	}
}

void ADialogue::SetupDialogue(UDialogueTreeData* InDialogueTree, AToy* InParent)
{
	DialogueTree = InDialogueTree;
	Parent = InParent;

	Initialize();
}

void ADialogue::Lock(bool bEnable)
{
	if (Parent)
	{
		Parent->Lock(bEnable);
	}
}

void ADialogue::OnAnswerQuestion_Implementation()
{
	Lock(true);
}

void ADialogue::CreateSubtitles()
{
	if (const APlayerController *PlayerController = GetWorld()->GetFirstPlayerController(); ensure(PlayerController))
	{
		if (const ALukaHUD* LukaHUD = Cast<ALukaHUD>(PlayerController->GetHUD()); ensure(LukaHUD))
		{
			USubtitlesWidget *SubtitlesWidget = LukaHUD->GetSubtitlesWidget();
			check(SubtitlesWidget);

			SubtitlesID = SubtitlesWidget->PushSubtitle();
			SetClarity(1.f);
		}
	}
}

void ADialogue::HandleMarker(FString const& Marker)
{
	FString const ModMarker = "MOD_";
	if (Marker.StartsWith(ModMarker)) // Then it has custom values
	{
		FString ChoppedMarker = Marker.Mid(ModMarker.Len());


		FString const EarlyAnswerMarker = "EARLYANSWER";
		if (ChoppedMarker.StartsWith(EarlyAnswerMarker))
		{
			if (DialogueTree)
			{
				OnFinishDialogue();
			}
			return;
		}

		return;
	}
	
	if (const APlayerController *PlayerController = GetWorld()->GetFirstPlayerController(); PlayerController)
	{
		if (const ALukaHUD* LukaHUD = Cast<ALukaHUD>(PlayerController->GetHUD()); LukaHUD)
		{
			USubtitlesWidget *SubtitlesWidget = LukaHUD->GetSubtitlesWidget();

			SubtitlesWidget->UpdateSubtitle(SubtitlesID, Marker);
		}
	}
}

void ADialogue::DestroySubtitles()
{
	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (ALukaHUD* HUD = Cast<ALukaHUD>(PlayerController->GetHUD()); ensure(HUD))
		{
			if (ensure(SubtitlesID != INVALID_ID))
			{
				HUD->GetSubtitlesWidget()->PopSubtitle(SubtitlesID);
			}
			SubtitlesID = INVALID_ID;
		}
	}
}

class UAkAudioEvent* ADialogue::GetAudioEvent() const
{
	return AudioEvent;
}

bool ADialogue::ProposeAnswer()
{
	if (bWaitingForPromptCheck)
	{
		bWaitingForPromptCheck = false;

		if (IsHeard())
		{
			ClosePrompt();
			OpenPrompt();
		}
		
		return true;
	}

	return false;
}

void ADialogue::OpenPrompt() const
{
	if (bWaitingForPromptCheck)
	{
		UTLUtils::TogglePrompt(GetWorld(), PromptWidgetName, true);
	} else
	{
		if (const APlayerController *PlayerController = GetWorld()->GetFirstPlayerController(); ensure(PlayerController))
		{
			if (const ALukaHUD* LukaHUD = Cast<ALukaHUD>(PlayerController->GetHUD()); LukaHUD)
			{
				UAnswerWidget *AnswerWidget = LukaHUD->GetAnswerWidget();
				AnswerWidget->Toggle(QuestionID);
			}
		}
	}
}


void ADialogue::ClosePrompt()
{
	UTLUtils::TogglePrompt(GetWorld(), PromptWidgetName, false);
	if (const APlayerController *PlayerController = GetWorld()->GetFirstPlayerController(); ensure(PlayerController))
	{
		if (const ALukaHUD* LukaHUD = Cast<ALukaHUD>(PlayerController->GetHUD()); LukaHUD)
		{
			UAnswerWidget *AnswerWidget = LukaHUD->GetAnswerWidget();
			AnswerWidget->Collapse();
		}
	}
}

bool ADialogue::IsHeard()
{
	return true;
}

void ADialogue::AskQuestion()
{
	if (!ensure(Parent))
	{
		return;
	}
	
	if (ensure(DialogueData)) // Shouldnt happen, wierd
	{
		if (!ensure(!DialogueData->IsAutoChoice())) // also shouldnt fail
		{
			return;
		}
		if (const APlayerController *PlayerController = GetWorld()->GetFirstPlayerController(); PlayerController)
		{
			if (const ALukaHUD* LukaHUD = Cast<ALukaHUD>(PlayerController->GetHUD()); LukaHUD)
			{
				const FAnswerList AnswerList = GetAnswerList(DialogueData);

				UAnswerWidget *AnswerWidget = LukaHUD->GetAnswerWidget();

				if (!AnswerWidget->IsValid(QuestionID))
				{
					QuestionID = AnswerWidget->Ask(AnswerList);

					if (!Parent->IsLocked())
					{
						bWaitingForPromptCheck = true;
					} else
					{
						bWaitingForPromptCheck = false;
					}
					if (IsHeard())
					{
						OpenPrompt();
					}
				}
				AnswerWidget->BindListener<ADialogue>(QuestionID, this, FName("OnAnswerSelected"));
			}
		}
	}
}

void ADialogue::EndDialogue()
{
	Lock(false);
	OnEndDialogue();

	DialogueExit.Execute(this);

	Destroy();
}

void ADialogue::OnEndDialogue_Implementation()
{
}

void ADialogue::OnAnswerSelected(FAnswer Answer)
{
	check(AkComponent)

	QuestionID = INVALID_ID;

	StepIntoDialogue(Answer.Next);

	// if (!StepNextDialogue())
	// {
	// 	StopDialogue();
	// }
}

void ADialogue::OnFinishDialogue()
{
	if (QuestionID != INVALID_ID)
	{
		return;
	}
	if (!StepNextDialogue())
	{
		EndDialogue();
	}
}

bool ADialogue::StepNextDialogue()
{
	if (DialogueData) 
	{
		if (DialogueData->GetAnswers().Num() == 0)
		{
			return false;
		}

		if (DialogueData->IsAutoChoice())
		{
			for (FAnswer const& Answer : DialogueData->GetAnswers())
			{
				TArray<TObjectPtr<UDialogueLineData>> const &Nodes = DialogueTree->Nodes;
				if (ensure(Nodes.Num() > Answer.Next))
				{
					StepIntoDialogue(Answer.Next);
					break;
				}
			}
			return true;
		}
		
		AskQuestion();
		return true;
	}
			
	return false;
}

void ADialogue::StepIntoDialogue(int32 ID)
{
	if (ID == INVALID_ID)
	{
		EndDialogue();
		return;
	}
	
	if (DialogueData) // end previous dialogue
	{
		if (DialogueData->GetStopAudioEvent())
		{
			PlayingID = AK_INVALID_PLAYING_ID;
			bEarlyDialogueFlag = true;
			IgnorePlayingID = AkComponent->PostAkEvent(DialogueData->GetStopAudioEvent());
		}
	}

	if (ensure(DialogueTree))
	{
		if (ensure(DialogueTree->Nodes.Num() > ID))
		{
			DialogueData = DialogueTree->Nodes[ID];
			AudioEvent = DialogueData->GetStartAudioEvent();

			if (AudioEvent)
			{
				PostAkEvent();
			} else
			{
				OnFinishDialogue();
				return;
			}
		}
	}

	OnAnswerQuestion();
}

void ADialogue::SetClarity(float Clarity)
{
	if (const APlayerController *PlayerController = GetWorld()->GetFirstPlayerController(); ensure(PlayerController))
	{
		if (const ALukaHUD* LukaHUD = Cast<ALukaHUD>(PlayerController->GetHUD()); ensure(LukaHUD))
		{
			USubtitlesWidget *SubtitlesWidget = LukaHUD->GetSubtitlesWidget();
			check(SubtitlesWidget);

			SubtitlesWidget->UpdateSubtitle(SubtitlesID, Clarity);
		}
	}
}
