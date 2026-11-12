// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperUnitPickupMesh.h"

#include "Components/FlipperUsableUnitComponent.h"
#include "Core/FlipperPlayerState.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFlipperUnitPickupMesh::AFlipperUnitPickupMesh()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	SetRootComponent(StaticMeshComponent);
}

void AFlipperUnitPickupMesh::OnRep_PickedUp()
{
	StaticMeshComponent->SetVisibility(!bIsPickedUp);
	StaticMeshComponent->SetCollisionEnabled(bIsPickedUp ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
}

void AFlipperUnitPickupMesh::Interact_Implementation(AActor* Interactor)
{
	if (!HasAuthority()) return;
	if (bIsPickedUp) return;
	
	UFlipperUsableUnitComponent* UnitsComponent = GetActiveUnitsComponent(Interactor);
	if (!UnitsComponent) return;
	
	// attempt to give the right amount of units to the player, this gates overflowing so don't need to manually check free space
	if (!UnitsComponent->TryAddUnit(UnitsToGive)) return;
	
	bIsPickedUp = true;
	
	// increment the number of picked up units for the interactor
	AFlipperPlayerState* InteractorPlayerState = GetInteractorPlayerState(Interactor);
	if (InteractorPlayerState)
	{
		InteractorPlayerState->IncrementPickedUpCount(UnitType);
	}
	
	OnPickupMeshCompleted.Broadcast(this, Cast<APawn>(Interactor));
	OnRep_PickedUp();
}

FFlipperInteractText AFlipperUnitPickupMesh::GetInteractText(AActor* Interactor)
{
	FFlipperInteractText NewInteractText;

	const UFlipperUsableUnitComponent* UnitsComponent = GetActiveUnitsComponent(Interactor);
	if (!UnitsComponent)
	{
		// if there's an error getting information about surface data or the units, show a generic message
		NewInteractText.MainText = FText::FromString("Incorrect Tool");
		
		UFlipperGameInstance* FlipperGameInstance = GetFlipperGameInstance();
		if (FlipperGameInstance)
		{
			const FText DesiredToolDisplayName = FlipperGameInstance->ToolNames.Find(UnitType)->ToLower();
			NewInteractText.DescriptionText = FText::Format(FText::FromString("Equip your {0}"), DesiredToolDisplayName);
		}
		
		return NewInteractText;
	}
	
	NewInteractText.MainText = FText::FromString("Pick Up");
	
	// show "(you don't have space)" if the user doesn't have enough space in their units component to fit this pickup in
	const uint8 FreeSpace = UnitsComponent->GetMaxUsableUnits() - UnitsComponent->GetUsableUnits();
	if (FreeSpace < UnitsToGive)
	{
		NewInteractText.DescriptionText = FText::Format(FText::FromString("Weight: {0} {0}|plural(one=unit,other=units) (you don't have space)"), UnitsToGive);
	}
	else
	{
		NewInteractText.DescriptionText = FText::Format(FText::FromString("Weight: {0} {0}|plural(one=unit,other=units)"), UnitsToGive);
	}
	
	return NewInteractText;
}

void AFlipperUnitPickupMesh::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlipperUnitPickupMesh, bIsPickedUp)
}

