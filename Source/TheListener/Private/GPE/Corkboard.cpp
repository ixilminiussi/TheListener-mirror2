// Fill out your copyright notice in the Description page of Project Settings.


#include "GPE/Corkboard.h"

#include "EnhancedInputComponent.h"
#include "Components/BoxComponent.h"
#include "GPE/Toy.h"
#include "Kismet/GameplayStatics.h"
#include "UI/CursorWidget.h"
#include "UI/LukaHUD.h"
#include "UI/PlayWidget.h"
#include "UI/Corkboard/CorkboardWidget.h"

class ALukaHUD;

ACorkboard::ACorkboard()
{
	BoardMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Board Mesh");
	check(BoardMeshComponent);

	BoardMeshComponent->SetupAttachment(CollisionComponent);
}

void ACorkboard::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}


void ACorkboard::BeginPlay()
{
	Super::BeginPlay();
	CorkboardWidget = GetCorkboardWidget();
}

void ACorkboard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (!ensure(EnhancedInputComponent != nullptr)) { return; }
	if (ensure(InputActionMove != nullptr))
	{
		EnhancedInputComponent->BindAction(InputActionMove, ETriggerEvent::Triggered, CorkboardWidget,
		                                   &UCorkboardWidget::MoveInput);
		EnhancedInputComponent->BindAction(InputActionMove, ETriggerEvent::None, CorkboardWidget,
		                                   &UCorkboardWidget::MoveInput);
	}
	if (ensure(InputActionZoom != nullptr))
	{
		EnhancedInputComponent->BindAction(InputActionZoom, ETriggerEvent::Triggered, CorkboardWidget,
		                                   &UCorkboardWidget::ZoomInput);
		EnhancedInputComponent->BindAction(InputActionZoom, ETriggerEvent::None, CorkboardWidget,
		                                   &UCorkboardWidget::ZoomInput);
	}
}

void ACorkboard::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController != UGameplayStatics::GetPlayerController(GetWorld(), 0)) { return; }
	GetCorkboardWidget()->SetVisibility(ESlateVisibility::Visible);
}

void ACorkboard::UnPossessed()
{
	Super::UnPossessed();
	CorkboardWidget->SetVisibility(ESlateVisibility::Hidden);
	CorkboardWidget->Reset();
}

void ACorkboard::UpdateClue(class UClueAsset* Clue)
{
	if (CorkboardWidget == nullptr) { CorkboardWidget = GetCorkboardWidget(); }
	CorkboardWidget->UpdateClue(Clue);
}

UCorkboardWidget* ACorkboard::GetCorkboardWidget()
{
	if (CorkboardWidget != nullptr) { return CorkboardWidget; }
	UCorkboardWidget* CorkboardWidgetTemp = CreateWidget<UCorkboardWidget>(
		GetWorld()->GetFirstPlayerController(), CorkboardWidgetClass.Get());
	check(CorkboardWidgetTemp);
	CorkboardWidgetTemp->AddToViewport();
	CorkboardWidgetTemp->SetVisibility(ESlateVisibility::Hidden);
	CorkboardWidgetTemp->SetMoveSpeed(MoveSpeed);
	CorkboardWidgetTemp->SetZoomSpeed(ZoomSpeed);
	CorkboardWidgetTemp->SetZoomLimits(ZoomLimits);
	CorkboardWidgetTemp->SetupNoteWidgets();
	return CorkboardWidgetTemp;
}
