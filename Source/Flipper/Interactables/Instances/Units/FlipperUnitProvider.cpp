// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperUnitProvider.h"

#include "Components/FlipperUsableUnitComponent.h"
#include "Core/FlipperGameInstance.h"
#include "Core/FlipperGameState.h"
#include "Core/FlipperPlayerState.h"
#include "DataAssets/FlipperSurfaceFinishData.h"


// Sets default values
AFlipperUnitProvider::AFlipperUnitProvider()
{
	bReplicates = true;

	FillStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FillStaticMeshComponent"));
}

void AFlipperUnitProvider::SetupMaterial()
{
	if (FillMaterial)
	{
		FillMaterialDynamic = UMaterialInstanceDynamic::Create(FillMaterial, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FillMaterial is null on: %s"), *this->GetName());
	}
	
	if (FillStaticMeshComponent && FillMaterialDynamic)
	{
		FillStaticMeshComponent->SetMaterial(0, FillMaterialDynamic);
		
		// ensure default material parameters are applied
		const UFlipperSurfaceFinishData* SurfaceFinishData = GetSurfaceData();
		if (SurfaceFinishData)
		{
			if (!ProvidedFinish.IsValid()) return; // cancel if there's no provided finish set
			
			// get the starting provided finish from the game instance data asset and ensure the material is set up with that information
			const FFlipperSurfaceFinish* ProvidedSurfaceFinish = SurfaceFinishData->GetFinish(ProvidedFinish);
			if (ProvidedSurfaceFinish)
			{
				// update the colour and texture with whatever surface finish is given out by this unit provider
				FillMaterialDynamic->SetVectorParameterValue(MaterialProvidedColourParameterName, ProvidedSurfaceFinish->SurfaceColour);
			
				UTexture* ProvidedTexture = SurfaceFinishData->GetMaterialTexture(ProvidedSurfaceFinish->MaterialTexture);
				if (ProvidedTexture)
				{
					FillMaterialDynamic->SetTextureParameterValue(MaterialProvidedTextureParameterName, ProvidedTexture);
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void AFlipperUnitProvider::BeginPlay()
{
	Super::BeginPlay();
	
	// set up the material before authority check
	// so that each client shows the material on its own copy of the actor
	SetupMaterial();
}

void AFlipperUnitProvider::Interact_Implementation(AActor* Interactor)
{
	if (!HasAuthority()) return;
	if (!ProvidedFinish.IsValid()) return;
	
	UWorld* World = GetWorld();
	if (!World) return;
	
	AFlipperGameState* FlipperGameState = Cast<AFlipperGameState>(World->GetGameState());
	if (!FlipperGameState) return;
	
	const UFlipperSurfaceFinishData* SurfaceFinishData = GetSurfaceData();
	const FFlipperSurfaceFinish* ProvidedSurfaceFinish = SurfaceFinishData ? SurfaceFinishData->GetFinish(ProvidedFinish) : nullptr;
	if (!ProvidedSurfaceFinish) return;
	
	UFlipperUsableUnitComponent* UnitsComponent = GetActiveUnitsComponent(Interactor);
	if (!UnitsComponent) return;

	// don't allow swapping finishes while holding units of a different finish
	if (UnitsComponent->GetUsableUnits() > 0 && UnitsComponent->GetCurrentFinish() != ProvidedFinish) return;
	
	// work out if the team can afford the purchase of units to the interactor
	const uint8 FreeSpace = UnitsComponent->GetMaxUsableUnits() - UnitsComponent->GetUsableUnits();
	if (FreeSpace == 0) return; // don't allow refilling an already full usable unit component
	
	const int32 TotalCost = ProvidedSurfaceFinish->UnitPrice * static_cast<int32>(FreeSpace);  // cast FreeSpace(uint8) to int32 so total cost isn't capped to 255
	
	AFlipperPlayerState* InteractorPlayerState = nullptr;
	APawn* InteractorPawn = Cast<APawn>(Interactor);
	if (InteractorPawn)
	{
		InteractorPlayerState = InteractorPawn->GetPlayerState<AFlipperPlayerState>();
	}
	
	// if the team can't afford it don't change anything
	if (TotalCost > 0 && !FlipperGameState->TrySpendFunds(TotalCost, InteractorPlayerState)) return;
	
	// set the finish BEFORE adding units, as TrySetCurrentFinish reads whether UsableUnits is more than 0, so adding units first would make the purchase fail
	// 
	// OR operator evaluates left to right so TryAddUnit only runs if the finish was set successfully
	// https://eel.is/c++draft/expr.log.or "Unlike |, || guarantees left-to-right evaluation; moreover, the second operand is not evaluated if the first operand evaluates to true."
	if (!UnitsComponent->TrySetCurrentFinish(ProvidedFinish) || !UnitsComponent->TryAddUnit(FreeSpace))
	{
		FlipperGameState->RefundFunds(TotalCost, InteractorPlayerState);
	}
}

