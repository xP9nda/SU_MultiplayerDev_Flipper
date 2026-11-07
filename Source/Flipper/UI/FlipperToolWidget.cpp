// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperToolWidget.h"

#include "Components/FlipperToolComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UFlipperToolWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (!ToolImageWidget || !ToolImageTexture) return;
	ToolImageWidget->SetBrushFromTexture(ToolImageTexture);
	
	if (!ToolKeyTextBlock) return;
	ToolKeyTextBlock->SetText(ToolKeyText);
}

void UFlipperToolWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	AController* OwningController = GetOwningPlayer();
	if (!OwningController) return;
	
	OwningController->OnPossessedPawnChanged.AddUniqueDynamic(this, &UFlipperToolWidget::HandlePossessedPawnChanged);
	
	const APawn* OwningPawn = OwningController->GetPawn();
	ObserveComponent(OwningPawn ? OwningPawn->FindComponentByClass<UFlipperToolComponent>() : nullptr);
}

void UFlipperToolWidget::NativeDestruct()
{
	AController* OwningController = GetOwningPlayer();
	if (OwningController)
	{
		OwningController->OnPossessedPawnChanged.RemoveDynamic(this, &UFlipperToolWidget::HandlePossessedPawnChanged);
	}
	
	ObserveComponent(nullptr);
	
	Super::NativeDestruct();
}

void UFlipperToolWidget::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	ObserveComponent(NewPawn ? NewPawn->FindComponentByClass<UFlipperToolComponent>() : nullptr);
}

void UFlipperToolWidget::ObserveComponent(UFlipperToolComponent* NewComponent)
{
	if (IsValid(ObservedToolComponent))
	{
		ObservedToolComponent->OnToolChanged.RemoveDynamic(this, &UFlipperToolWidget::HandleToolChanged);
	}
	
	ObservedToolComponent = NewComponent;
	
	if (IsValid(ObservedToolComponent))
	{
		ObservedToolComponent->OnToolChanged.AddUniqueDynamic(this, &UFlipperToolWidget::HandleToolChanged);
		HandleToolChanged(ObservedToolComponent->GetCurrentTool()); // read the current value and show it right away for starting state
	}
	else
	{
		// no tool component found, so nothing is selected
		OnSelectedToolChanged(false);
	}
}

void UFlipperToolWidget::HandleToolChanged(FGameplayTag NewTool)
{
	OnSelectedToolChanged(NewTool.MatchesTagExact(ToolUnitType));
}
