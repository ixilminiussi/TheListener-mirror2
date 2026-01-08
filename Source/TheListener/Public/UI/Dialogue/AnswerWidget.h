// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnswerDataAsset.h"
#include "CommonUserWidget.h"
#include "AnswerWidget.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAnswer, FAnswer, Answer);

USTRUCT()
struct FAnswerOptionsContainer
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<class UAnswerButtonWidget*> AnswerButtons;

	FTimerHandle ProgressHandle;
	FTimerHandle TimeoutHandle;

	FAnswerList AnswerList;
};
/**
 * 
 */
UCLASS()
class THELISTENER_API UAnswerWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	int32 Ask(struct FAnswerList const &);

	void Toggle(int32 QuestionID);
	void Collapse();

	bool IsValid(int32 QuestionID);

	/**
	 * Can only bind one function at a time
	 */
	template<typename UserClass>
	void BindListener(int32 QuestionID, UserClass* Object, FName FuncName);

protected:
	void OnTimerUpdate(int32 QuestionID) const;
	void SelectAnswer(int32 QuestionID, int32 Choice);

	UPROPERTY(EditDefaultsOnly, Category = "Answers")
	float Spacing;

	UPROPERTY(EditDefaultsOnly, Category = "Answers")
	TSubclassOf<class UAnswerButtonWidget> AnswerButtonClass;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UVerticalBox> ButtonBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UProgressBar> ProgressBar;

	UPROPERTY()
	TMap<int32, FAnswerOptionsContainer> ChoiceContainers;

	UPROPERTY()
	TMap<int32, FOnAnswer> OnAnswerEvents;

	int32 ToggledQuestion = -1;
};

template<typename UserClass>
	inline void UAnswerWidget::BindListener(int32 QuestionID, UserClass* Object, FName FuncName)
{
	if (ensure(OnAnswerEvents.Contains(QuestionID)))
	{
		OnAnswerEvents[QuestionID].BindUFunction(Object, FuncName);
	}
}
