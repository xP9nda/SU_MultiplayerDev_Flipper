// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperUnitsWidget.h"

#include "FlipperCharacter.h"
#include "FlipperGameInstance.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UFlipperUnitsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	AController* OwningController = GetOwningPlayer();
	AActor* OwningPawn = OwningController->GetPawn();
	
	// bind to a new pawn, so that if the character is slow to spawn in, the component delegate bindings are still set up
	OwningController->OnPossessedPawnChanged.AddDynamic(this, &UFlipperUnitsWidget::HandlePossessedPawnChanged);
	
	// attempt to set bindings up already if the component can be found, but keep the above binding as fallback security
	ObservedUnitComponent = UFlipperUsableUnitComponent::FindUnitComponentOfType(OwningPawn, UnitType);
	if (!ObservedUnitComponent)
	{
		// cancel out and hide the widget if no matching unit type usable unit component was found
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	ObservedUnitComponent->OnUsableUnitsChanged.AddDynamic(this, &UFlipperUnitsWidget::UpdateDisplay);
	ObservedUnitComponent->OnCurrentFinishChanged.AddDynamic(this, &UFlipperUnitsWidget::UpdateDisplay);
	
	// don't plan on setting a default for the usable unit components, but just in case, the values should be reflected
	UpdateDisplay();
	
	UpdateVisibility();
}

void UFlipperUnitsWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	UnitIcon->SetBrushFromTexture(UnitIconTexture);
	
	// don't hide the widget when it's displayed in the designer
	if (!IsDesignTime())
	{
		UpdateVisibility();
	}
}

void UFlipperUnitsWidget::HandlePossessedPawnChanged(APawn* NewPawn, APawn* OldPawn)
{
	if (ObservedUnitComponent)
	{
		ObservedUnitComponent->OnUsableUnitsChanged.RemoveDynamic(this, &UFlipperUnitsWidget::UpdateDisplay);
		ObservedUnitComponent->OnCurrentFinishChanged.RemoveDynamic(this, &UFlipperUnitsWidget::UpdateDisplay);
	}
	
	ObservedUnitComponent = UFlipperUsableUnitComponent::FindUnitComponentOfType(NewPawn, UnitType);
	if (ObservedUnitComponent)
	{
		ObservedUnitComponent->OnUsableUnitsChanged.AddDynamic(this, &UFlipperUnitsWidget::UpdateDisplay);
		ObservedUnitComponent->OnCurrentFinishChanged.AddDynamic(this, &UFlipperUnitsWidget::UpdateDisplay);
		
		// immediately trigger a display update as pawn has just changed
		UpdateDisplay();
	}
	
	UpdateVisibility();
}

void UFlipperUnitsWidget::UpdateVisibility()
{
	// hide the widget if there's no usable unit component, or if the unit type is empty
	SetVisibility(ObservedUnitComponent && UnitType != FGameplayTag::EmptyTag ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UFlipperUnitsWidget::UpdateDisplay()
{
	const uint8 CurrentUnits = ObservedUnitComponent->GetUsableUnits();
	const uint8 MaxUsableUnits = ObservedUnitComponent->GetMaxUsableUnits();
	const FGameplayTag CurrentFinish = ObservedUnitComponent->GetCurrentFinish();
	
	// update the text labels
	CurrentUnitsAvailableText->SetText(
		FText::Format(
			FText::FromString("{0}"),
			FText::AsNumber(CurrentUnits)
		)
	);
	
	MaxUnitsAvailableText->SetText(
		FText::Format(
			FText::FromString("/{0}"),
			FText::AsNumber(MaxUsableUnits)
		)
	);
	
	// update the progress bar
	// avoid the div by 0 by setting it manually if the current units is 0
	UnitsProgressBar->SetPercent(CurrentUnits == 0 ? 0.0f : static_cast<float>(CurrentUnits) / static_cast<float>(MaxUsableUnits));
	
	// update the current finish info (progress bar colour and finish display name)
	const UFlipperGameInstance* GameInstance = Cast<UFlipperGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance && GameInstance->SurfaceData)
	{
		const FFlipperSurfaceFinish* FinishData = GameInstance->SurfaceData->GetFinish(CurrentFinish);
		if (FinishData)
		{
			CurrentFinishDisplayNameText->SetText(CurrentUnits == 0 ? FText::FromString("-") : FinishData->DisplayName);
			UnitsProgressBar->SetFillColorAndOpacity(FinishData->SurfaceColour);
		} else
		{
			CurrentFinishDisplayNameText->SetText(FText::FromString("-"));
		}
	}
	
	UpdateVisibility();
}


