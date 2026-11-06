// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperUnitSeller.h"

#include "Components/FlipperToolComponent.h"
#include "Components/FlipperUsableUnitComponent.h"
#include "Core/FlipperGameState.h"
#include "DataAssets/FlipperSurfaceFinishData.h"


// Sets default values
AFlipperUnitSeller::AFlipperUnitSeller()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}


void AFlipperUnitSeller::Interact_Implementation(AActor* Interactor)
{
	if (!HasAuthority()) return;
	
	AFlipperGameState* FlipperGameState = GetFlipperGameState();
	if (!FlipperGameState) return;
	
	FFlipperSaleInformation SaleInfo;
	if (!TryGetSaleInformation(Interactor, SaleInfo)) return;
	
	// take the units before refunding, so that a failed unit removal doesn't give money
	if (!SaleInfo.UnitsComponent->TryConsumeUnit(SaleInfo.UnitsToSell)) return;
	
	// refund the amount of money so that it's removed from the money that the player spent and doesn't contribute to money earned
	// players would read "money earned" as the amount they've received from gameplay actions, this just corrects their spending to remove the refunded value
	FlipperGameState->RefundFunds(SaleInfo.TotalRefundValue, GetInteractorPlayerState(Interactor));
}

FFlipperInteractText AFlipperUnitSeller::GetInteractText(AActor* Interactor)
{
	FFlipperInteractText NewInteractText = FFlipperInteractText();
	
	FFlipperSaleInformation SaleInfo;
	if (!TryGetSaleInformation(Interactor, SaleInfo))
	{
		NewInteractText.MainText = FText::FromString("Sell Units");
		
		UFlipperGameInstance* FlipperGameInstance = GetFlipperGameInstance();
		UFlipperToolComponent* InteractorToolComponent = Interactor->FindComponentByClass<UFlipperToolComponent>();
		if (InteractorToolComponent)
		{
			FText ToolDisplayName = FlipperGameInstance->ToolNames.Find(InteractorToolComponent->GetCurrentTool())->ToLower();
			NewInteractText.DescriptionText = FText::Format(FText::FromString("Your {0} is empty"), ToolDisplayName);
		}
		else
		{
			NewInteractText.DescriptionText = FText::FromString("You must be holding units to sell");
		}
		
		return NewInteractText;
	}
	
	NewInteractText.MainText = FText::Format(
		FText::FromString("Sell {0} Units of {1}"), 
		SaleInfo.UnitsToSell,
		SaleInfo.HeldSurfaceFinish->DisplayName
	);
	
	FCulturePtr USCulture = FInternationalization::Get().GetCulture(TEXT("en-US")); // strip the "US" from "US$x.xx"
	FText FormattedTotalRefundValue = FText::AsCurrencyBase(SaleInfo.TotalRefundValue, TEXT("USD"), USCulture);
	
	NewInteractText.DescriptionText = FText::Format(
		FText::FromString("{0} sell value ({1}% of original cost)"), 
		FormattedTotalRefundValue,
		SellPercentage * 100
	);
	
	return NewInteractText;
}

bool AFlipperUnitSeller::TryGetSaleInformation(AActor* Interactor, FFlipperSaleInformation& SaleInfo) const
{
	// get the tool component and usable unit component manually rather than through the inherited helper,
	// as this sell point supports all unit types, driven by the tool the user currently has equipped
	const UFlipperToolComponent* ToolComponent = Interactor ? Interactor->FindComponentByClass<UFlipperToolComponent>() : nullptr;
	if (!ToolComponent) return false;
	
	SaleInfo.UnitsComponent = UFlipperUsableUnitComponent::FindUnitComponentOfType(Interactor, ToolComponent->GetCurrentTool());
	if (!SaleInfo.UnitsComponent) return false;
	
	SaleInfo.UnitsToSell = SaleInfo.UnitsComponent->GetUsableUnits();
	if (SaleInfo.UnitsToSell == 0) return false;
	
	const UFlipperSurfaceFinishData* SurfaceFinishData = GetSurfaceData();
	SaleInfo.HeldSurfaceFinish = SurfaceFinishData ? SurfaceFinishData->GetFinish(SaleInfo.UnitsComponent->GetCurrentFinish()) : nullptr;
	if (!SaleInfo.HeldSurfaceFinish) return false;
	
	// round the total refund value down to an int as you can't get more fractional than pennies
	SaleInfo.TotalRefundValue = FMath::FloorToInt32(static_cast<float>(SaleInfo.UnitsToSell) * static_cast<float>(SaleInfo.HeldSurfaceFinish->UnitPrice) * SellPercentage);

	// if total refund value is 0, return false, otherwise everything passes, sale info is valid and therefore return true
	return SaleInfo.TotalRefundValue > 0;
}
