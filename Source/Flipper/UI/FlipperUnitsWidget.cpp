// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperUnitsWidget.h"

#include "FlipperGameInstance.h"
#include "Components/FlipperUsableUnitComponent.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UFlipperUnitsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	AController* OwningController = GetOwningPlayer();
	if (!OwningController)
	{
		UpdateVisibility();
		return;
	}
	
	// bind to a new pawn, so that if the character is slow to spawn in, the component delegate bindings are still set up
	OwningController->OnPossessedPawnChanged.AddUniqueDynamic(this, &UFlipperUnitsWidget::HandlePossessedPawnChanged);
	ObserveComponent(UFlipperUsableUnitComponent::FindUnitComponentOfType(OwningController->GetPawn(), UnitType));
}

void UFlipperUnitsWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (IsValid(UnitIcon) && IsValid(UnitIconTexture))
	{
		UnitIcon->SetBrushFromTexture(UnitIconTexture);
	}
	
	// don't hide the widget when it's displayed in the designer
	if (!IsDesignTime())
	{
		UpdateVisibility();
	}
}

void UFlipperUnitsWidget::NativeDestruct()
{
	AController* OwningController = GetOwningPlayer();
	if (OwningController)
	{
		// unbind the pawn changed delegate
		OwningController->OnPossessedPawnChanged.RemoveDynamic(this, &UFlipperUnitsWidget::HandlePossessedPawnChanged);
	}
	
	// force no new component and therefore a clean-up on the old component bindings
	ObserveComponent(nullptr);
	
	Super::NativeDestruct();
}

void UFlipperUnitsWidget::ObserveComponent(UFlipperUsableUnitComponent* NewComponent)
{
	// if a component was being observed, unbind its delegates
	if (IsValid(ObservedUnitComponent))
	{
		ObservedUnitComponent->OnUsableUnitsChanged.RemoveDynamic(this, &UFlipperUnitsWidget::UpdateDisplay);
		ObservedUnitComponent->OnCurrentFinishChanged.RemoveDynamic(this, &UFlipperUnitsWidget::UpdateDisplay);
	}
	
	// update the observed unit component to the new one and re-bind the new delegates if it's valid
	ObservedUnitComponent = NewComponent;
	if (IsValid(ObservedUnitComponent))
	{
		ObservedUnitComponent->OnUsableUnitsChanged.AddUniqueDynamic(this, &UFlipperUnitsWidget::UpdateDisplay);
		ObservedUnitComponent->OnCurrentFinishChanged.AddUniqueDynamic(this, &UFlipperUnitsWidget::UpdateDisplay);
		
		// valid new component, so immediately update the visuals
		UpdateDisplay();
	}
	
	UpdateVisibility();
}

void UFlipperUnitsWidget::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	ObserveComponent(UFlipperUsableUnitComponent::FindUnitComponentOfType(NewPawn, UnitType));
}

void UFlipperUnitsWidget::UpdateVisibility()
{
	// hide the widget if there's no usable unit component, or if the unit type is empty
	SetVisibility(ObservedUnitComponent && UnitType.IsValid() ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UFlipperUnitsWidget::UpdateDisplay()
{
	if (!IsValid(ObservedUnitComponent)) return;
	
	const uint8 CurrentUnits = ObservedUnitComponent->GetUsableUnits();
	const uint8 MaxUsableUnits = ObservedUnitComponent->GetMaxUsableUnits();
	const FGameplayTag CurrentFinish = ObservedUnitComponent->GetCurrentFinish();
	
	// update the text labels
	CurrentUnitsAvailableText->SetText(FText::AsNumber(CurrentUnits));
	MaxUnitsAvailableText->SetText(FText::Format(FText::FromString("/{0}"), FText::AsNumber(MaxUsableUnits)));
	
	// update the progress bar
	// avoid the div by 0 by setting it manually if any unit values are 0
	UnitsProgressBar->SetPercent(CurrentUnits == 0 || MaxUsableUnits == 0 ? 0.0f : static_cast<float>(CurrentUnits) / static_cast<float>(MaxUsableUnits));
	
	// update the current finish info (progress bar colour and finish display name)
	const UFlipperGameInstance* GameInstance = Cast<UFlipperGameInstance>(GetWorld()->GetGameInstance());
	
	if (!GameInstance || !GameInstance->SurfaceData)
	{
		UE_LOG(LogTemp, Warning, TEXT("SurfaceData in GameInstance is null"));
		CurrentFinishDisplayNameText->SetText(FText::FromString("-"));
		return;
	}
	
	const FFlipperSurfaceFinish* FinishData = GameInstance->SurfaceData->GetFinish(CurrentFinish);
	if (FinishData)
	{
		CurrentFinishDisplayNameText->SetText(CurrentUnits == 0 ? FText::FromString("-") : FinishData->DisplayName);
		UnitsProgressBar->SetFillColorAndOpacity(FinishData->SurfaceColour);
	} else
	{
		CurrentFinishDisplayNameText->SetText(FText::FromString("-"));
		
		if (!CurrentFinish.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("FlipperUnitsWidget: CurrentFinish (%s) is missing from GameInstance->SurfaceData"), *CurrentFinish.ToString());
		}
	}
	
	
	UpdateVisibility();
}


