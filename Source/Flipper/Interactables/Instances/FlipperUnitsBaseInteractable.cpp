// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperUnitsBaseInteractable.h"

#include "Components/FlipperToolComponent.h"
#include "Components/FlipperUsableUnitComponent.h"


// Sets default values
AFlipperUnitsBaseInteractable::AFlipperUnitsBaseInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AFlipperUnitsBaseInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

UFlipperUsableUnitComponent* AFlipperUnitsBaseInteractable::GetActiveUnitsComponent(AActor* Interactor) const
{
	const UFlipperToolComponent* ToolComponent = Interactor ? Interactor->FindComponentByClass<UFlipperToolComponent>() : nullptr;
	if (!ToolComponent || !ToolComponent->GetCurrentTool().MatchesTagExact(UnitType)) return nullptr;
	
	// only return the usable unit component if the current tool matches the unit type
	return UFlipperUsableUnitComponent::FindUnitComponentOfType(Interactor, UnitType);
}

// Called every frame
void AFlipperUnitsBaseInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

