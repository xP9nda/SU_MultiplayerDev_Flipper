// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperUnitsWidget.h"

#include "Components/FlipperToolComponent.h"
#include "Components/FlipperUsableUnitComponent.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Core/FlipperGameInstance.h"

void UFlipperUnitsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (UnitsProgressMaterial)
	{
		UnitsProgressMaterialDynamic = UMaterialInstanceDynamic::Create(UnitsProgressMaterial, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UnitsProgressMaterial is null on: %s"), *this->GetName());
	}
	
	if (UnitsProgressMaterialDynamic.IsValid() && UnitsProgressImage)
	{
		UnitsProgressImage->SetBrushFromMaterial(UnitsProgressMaterialDynamic.Get());
	}
	
	AController* OwningController = GetOwningPlayer();
	if (!OwningController) return;
	
	// bind to a new pawn, so that if the character is slow to spawn in, the component delegate bindings are still set up
	OwningController->OnPossessedPawnChanged.AddUniqueDynamic(this, &UFlipperUnitsWidget::HandlePossessedPawnChanged);
	HandlePossessedPawnChanged(nullptr, OwningController->GetPawn());
}

void UFlipperUnitsWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
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
	ObserveToolComponent(nullptr);
	ObserveUnitsComponent(nullptr);
	
	Super::NativeDestruct();
}

void UFlipperUnitsWidget::ObserveUnitsComponent(UFlipperUsableUnitComponent* NewComponent)
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
	
	if (!IsValid(ObservedUnitComponent))
	{
		// if there's no units component then clear the display
		// otherwise old tool values might still appear on screen
		ClearDisplay();
	}
}

void UFlipperUnitsWidget::ObserveToolComponent(UFlipperToolComponent* NewComponent)
{
	if (IsValid(ObservedToolComponent))
	{
		ObservedToolComponent->OnToolChanged.RemoveDynamic(this, &UFlipperUnitsWidget::HandleToolChanged);
	}
	
	ObservedToolComponent = NewComponent;
	if (IsValid(ObservedToolComponent))
	{
		ObservedToolComponent->OnToolChanged.AddUniqueDynamic(this, &UFlipperUnitsWidget::HandleToolChanged);
		HandleToolChanged(ObservedToolComponent->GetCurrentTool());
	}
	else
	{
		// if there's no tool component then there's no units to show, so unbind the unit component too
		ObserveUnitsComponent(nullptr);
	}
}

void UFlipperUnitsWidget::HandleToolChanged(FGameplayTag NewTool)
{
	if (!IsValid(ObservedToolComponent)) return;
	
	const UFlipperGameInstance* GameInstance = Cast<UFlipperGameInstance>(GetWorld()->GetGameInstance());
	const FText* CurrentToolName = GameInstance ? GameInstance->ToolNames.Find(NewTool) : nullptr;
	CurrentToolDisplayNameText->SetText(CurrentToolName ? *CurrentToolName : FText::FromString("-"));
	
	ObserveUnitsComponent(UFlipperUsableUnitComponent::FindUnitComponentOfType(ObservedToolComponent->GetOwner(), NewTool));
}

void UFlipperUnitsWidget::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	ObserveToolComponent(NewPawn ? NewPawn->GetComponentByClass<UFlipperToolComponent>() : nullptr);
}

void UFlipperUnitsWidget::ClearDisplay()
{
	CurrentUnitsAvailableText->SetText(FText::FromString("-"));
	MaxUnitsAvailableText->SetText(FText::FromString("-"));
	CurrentFinishDisplayNameText->SetText(FText::FromString("-"));
	
	if (UnitsProgressMaterialDynamic.Get())
	{
		UnitsProgressMaterialDynamic.Get()->SetScalarParameterValue(MaterialFillProgressParameterName, 0.0f);
	}
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
	float FillPercent = CurrentUnits == 0 || MaxUsableUnits == 0 ? 0.0f : static_cast<float>(CurrentUnits) / static_cast<float>(MaxUsableUnits);
	if (UnitsProgressMaterialDynamic.Get())
	{
		UnitsProgressMaterialDynamic.Get()->SetScalarParameterValue(MaterialFillProgressParameterName, FillPercent);
	}
	
	const UFlipperGameInstance* GameInstance = Cast<UFlipperGameInstance>(GetWorld()->GetGameInstance());
	if (!GameInstance) return;
	
	// update the current finish info (progress bar colour and finish display name)
	if (!GameInstance->SurfaceData)
	{
		UE_LOG(LogTemp, Warning, TEXT("SurfaceData in GameInstance is null"));
		CurrentFinishDisplayNameText->SetText(FText::FromString("-"));
		return;
	}
	
	// get the finish information about the current finish
	const FFlipperSurfaceFinish* FinishData = GameInstance->SurfaceData->GetFinish(CurrentFinish);
	if (FinishData)
	{
		CurrentFinishDisplayNameText->SetText(CurrentUnits == 0 ? FText::FromString("-") : FinishData->DisplayName);
		if (UnitsProgressMaterialDynamic.Get())
		{
			UnitsProgressMaterialDynamic.Get()->SetVectorParameterValue(MaterialColourParameterName, FinishData->SurfaceColour);
		}
	} else
	{
		CurrentFinishDisplayNameText->SetText(FText::FromString("-"));
		
		if (!CurrentFinish.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("FlipperUnitsWidget: CurrentFinish (%s) is missing from GameInstance->SurfaceData"), *CurrentFinish.ToString());
		}
	}
}


