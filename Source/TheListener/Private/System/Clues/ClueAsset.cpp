// Fill out your copyright notice in the Description page of Project Settings.


#include "System/Clues/ClueAsset.h"

#if UE_EDITOR
void UClueAsset::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PreviousLinks != Links)
	{
		Modify();
		UClueAsset* UpdatedClue = nullptr;
		for (UClueAsset* Link : Links)
		{
			if (PreviousLinks.Contains(Link)) { PreviousLinks.Remove(Link); }
			else { UpdatedClue = Link; }
		}

		if (UpdatedClue != nullptr)
		{
			UpdatedClue->Modify();
			UpdatedClue->Links.AddUnique(this);
			UpdatedClue->PreviousLinks = UpdatedClue->Links;
			UpdatedClue->MarkPackageDirty();
		}
		if (PreviousLinks.Num() > 0)
		{
			for (UClueAsset* Link : PreviousLinks)
			{
				if (Link == nullptr) { continue; }
				Link->Modify();
				Link->Links.Remove(this);
				Link->PreviousLinks = Link->Links;
				Link->MarkPackageDirty();
			}
		}

		PreviousLinks = Links;
		MarkPackageDirty();
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif
void UClueAsset::ResetClueState()
{
	State = EClueState::Hidden;
}


void UClueAsset::CheckClueState(bool hasBeenFound)
{
	//Return if the clue is unavailable
	if (State == EClueState::Disabled)
	{
		return;
	}
	if (!hasBeenFound && State == EClueState::Hidden) { return; }
	State = EClueState::Partial;
	for (UClueAsset* Link : Links)
	{
		if (Link->State == EClueState::Hidden) { return; }
		if (Link->State == EClueState::Disabled)
		{
			State = EClueState::Missing;
			return;
		}
	}
	State = EClueState::Complete;
}

void UClueAsset::MakeUnavailable()
{
	State = EClueState::Disabled;
}

bool UClueAsset::IsFound() const
{
	return (State == EClueState::Complete || State == EClueState::Partial);
}
