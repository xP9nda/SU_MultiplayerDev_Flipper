// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperInteractTextWidget.h"

#include "Components/FlipperInteractComponent.h"
#include "Components/TextBlock.h"
#include "Interfaces/FlipperInteract.h"

void UFlipperInteractTextWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	AController* OwningController = GetOwningPlayer();
	if (!OwningController) return;
	
	OwningController->OnPossessedPawnChanged.AddUniqueDynamic(this, &UFlipperInteractTextWidget::HandlePossessedPawnChanged);
	HandlePossessedPawnChanged(nullptr, OwningController->GetPawn());
}

void UFlipperInteractTextWidget::NativeDestruct()
{
	AController* OwningController = GetOwningPlayer();
	if (OwningController)
	{
		OwningController->OnPossessedPawnChanged.RemoveDynamic(this, &UFlipperInteractTextWidget::HandlePossessedPawnChanged);
	}
	
	ObserveComponent(nullptr);
	
	Super::NativeDestruct();
}

void UFlipperInteractTextWidget::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	ObserveComponent(NewPawn ? NewPawn->FindComponentByClass<UFlipperInteractComponent>() : nullptr);
}

void UFlipperInteractTextWidget::ObserveComponent(UFlipperInteractComponent* NewComponent)
{
	if (IsValid(ObservedComponent))
	{
		ObservedComponent->OnLookedAtActorChanged.RemoveDynamic(this, &UFlipperInteractTextWidget::HandleLookedAtActorChanged);
	}
	
	ObservedComponent = NewComponent;
	
	if (IsValid(ObservedComponent))
	{
		ObservedComponent->OnLookedAtActorChanged.AddDynamic(this, &UFlipperInteractTextWidget::HandleLookedAtActorChanged);
	}
	
	// immediately start hidden until the interaction component broadcasts what it's looking at
	HandleLookedAtActorChanged(nullptr);
}

void UFlipperInteractTextWidget::SetWidgetVisibility(bool bVisible)
{
	if (!bVisible)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UFlipperInteractTextWidget::HandleLookedAtActorChanged(AActor* NewLookedAtActor)
{
	LookedAtActor = NewLookedAtActor;
	RefreshInteractText();
	
	UWorld* World = GetWorld();
	if (!World) return;
	
	// keep refreshing while we are looking at something. while not looking at anything, clear the timer handle
	if (LookedAtActor.IsValid())
	{
		World->GetTimerManager().SetTimer(RefreshTimerHandle, this, &UFlipperInteractTextWidget::RefreshInteractText, RefreshInterval, true);
	}
	else
	{
		World->GetTimerManager().ClearTimer(RefreshTimerHandle);
	}
}


void UFlipperInteractTextWidget::RefreshInteractText()
{
	IFlipperInteract* Interact = Cast<IFlipperInteract>(LookedAtActor.Get());
	if (!Interact || !IsValid(ObservedComponent))
	{
		SetWidgetVisibility(false);
		return;
	}
	
	FFlipperInteractText InteractText = Interact->GetInteractText(ObservedComponent->GetOwner());
	
	// if both values are empty hide the widget
	if (InteractText.MainText.IsEmpty() && InteractText.DescriptionText.IsEmpty())
	{
		SetWidgetVisibility(false);
		return;
	}
	
	InteractMainText->SetText(InteractText.MainText);
	InteractDescriptionText->SetText(InteractText.DescriptionText);
	
	// update visibility of both main text and description text with whether any content exists
	InteractMainText->SetVisibility(!InteractText.MainText.IsEmpty() ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	InteractDescriptionText->SetVisibility(!InteractText.DescriptionText.IsEmpty() ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	
	SetWidgetVisibility(true);
}