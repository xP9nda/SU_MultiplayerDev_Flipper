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
	
	// get the tool component and usable unit component manually rather than through the inherited helper,
	// as this sell point supports all unit types, driven by the tool the user currently has equipped
	const UFlipperToolComponent* ToolComponent = Interactor ? Interactor->FindComponentByClass<UFlipperToolComponent>() : nullptr;
	if (!ToolComponent) return;
	
	UFlipperUsableUnitComponent* UnitsComponent = UFlipperUsableUnitComponent::FindUnitComponentOfType(Interactor, ToolComponent->GetCurrentTool());
	if (!UnitsComponent) return;
	
	const uint8 UnitsToSell = UnitsComponent->GetUsableUnits();
	if (UnitsToSell == 0) return;
	
	const UFlipperSurfaceFinishData* SurfaceFinishData = GetSurfaceData();
	const FFlipperSurfaceFinish* HeldSurfaceFinish = SurfaceFinishData ? SurfaceFinishData->GetFinish(UnitsComponent->GetCurrentFinish()) : nullptr;
	if (!HeldSurfaceFinish) return;
	
	// round the total refund value down to an int as you can't get more fractional than pennies
	const int32 TotalRefundValue = FMath::FloorToInt32(static_cast<float>(UnitsToSell) * static_cast<float>(HeldSurfaceFinish->UnitPrice) * SellPercentage);
	
	// take the units before refunding, so that a failed unit removal doesn't give money
	if (!UnitsComponent->TryConsumeUnit(UnitsToSell)) return;
	
	// refund the amount of money so that it's removed from the money that the player spent and doesn't contribute to money earned
	// players would read "money earned" as the amount they've received from gameplay actions, this just corrects their spending to remove the refunded value
	FlipperGameState->RefundFunds(TotalRefundValue, GetInteractorPlayerState(Interactor));
}
