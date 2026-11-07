// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperUnitsBaseInteractable.h"

#include "Components/FlipperToolComponent.h"
#include "Components/FlipperUsableUnitComponent.h"
#include "Core/FlipperGameInstance.h"
#include "Core/FlipperGameState.h"
#include "Core/FlipperPlayerState.h"
#include "GameFramework/Pawn.h"


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

// get the active units component for the held unit type, if the unit type doesn't match the held tool, returns nullptr
UFlipperUsableUnitComponent* AFlipperUnitsBaseInteractable::GetActiveUnitsComponent(AActor* Interactor) const
{
	const UFlipperToolComponent* ToolComponent = Interactor ? Interactor->FindComponentByClass<UFlipperToolComponent>() : nullptr;
	if (!ToolComponent || !ToolComponent->GetCurrentTool().MatchesTagExact(UnitType)) return nullptr;
	
	// only return the usable unit component if the current tool matches the unit type
	return UFlipperUsableUnitComponent::FindUnitComponentOfType(Interactor, UnitType);
}

AFlipperGameState* AFlipperUnitsBaseInteractable::GetFlipperGameState() const
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;
	
	return World->GetGameState<AFlipperGameState>();
}

AFlipperPlayerState* AFlipperUnitsBaseInteractable::GetInteractorPlayerState(AActor* Interactor) const
{
	const APawn* InteractorPawn = Cast<APawn>(Interactor);
	if (!InteractorPawn) return nullptr;
	
	AFlipperPlayerState* PlayerState = InteractorPawn->GetPlayerState<AFlipperPlayerState>();
	if (!PlayerState) return nullptr;
	
	return PlayerState;
}

const UFlipperSurfaceFinishData* AFlipperUnitsBaseInteractable::GetSurfaceData() const
{
	const UFlipperGameInstance* GameInstance = GetFlipperGameInstance();
	if (!GameInstance) return nullptr;
	
	return GameInstance->SurfaceData;
}

UFlipperGameInstance* AFlipperUnitsBaseInteractable::GetFlipperGameInstance() const
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;
	
	UFlipperGameInstance* GameInstance = World->GetGameInstance<UFlipperGameInstance>();
	return GameInstance;
}
