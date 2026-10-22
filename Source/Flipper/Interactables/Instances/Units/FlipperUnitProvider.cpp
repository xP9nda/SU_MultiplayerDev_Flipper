// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperUnitProvider.h"

#include "FlipperGameInstance.h"
#include "Components/FlipperUsableUnitComponent.h"
#include "Net/UnrealNetwork.h"


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
		const UFlipperGameInstance* GameInstance = Cast<UFlipperGameInstance>(GetWorld()->GetGameInstance());
		if (GameInstance && GameInstance->SurfaceData)
		{
			if (!ProvidedFinish.IsValid()) return; // cancel if there's no provided finish set
			
			// get the starting provided finish from the game instance data asset and ensure the material is set up with that information
			const FFlipperSurfaceFinish* ProvidedSurfaceFinish = GameInstance->SurfaceData->GetFinish(ProvidedFinish);
			if (ProvidedSurfaceFinish)
			{
				// update the colour and texture with whatever surface finish is given out by this unit provider
				FillMaterialDynamic->SetVectorParameterValue(MaterialProvidedColourParameterName, ProvidedSurfaceFinish->SurfaceColour);
				
				if (ProvidedSurfaceFinish->FinishedSurfaceTexture)
				{
					// have the unit provider adopt the look of the target texture, i.e. how the paintable mesh will look once it has been painted
					FillMaterialDynamic->SetTextureParameterValue(MaterialProvidedTextureParameterName, ProvidedSurfaceFinish->FinishedSurfaceTexture);
				}
				else
				{
					// if there's no target texture, then show the starting texture on the unit provider
					FillMaterialDynamic->SetTextureParameterValue(MaterialProvidedTextureParameterName, ProvidedSurfaceFinish->StartingSurfaceTexture);
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
	
	// on the server, set the unit provider to have it's maximum available units
	if (HasAuthority())
	{
		RemainingUnits = MaxUnitsAvailable;
	}
	
	// ensure late joiners see the current state of the unit provider right away
	OnRep_RemainingUnits();
}

void AFlipperUnitProvider::OnRep_RemainingUnits()
{
	// if all units have been used, hide the mesh
	FillStaticMeshComponent->SetVisibility(RemainingUnits > 0);
	
	// if there aren't any units left, guard against the division by 0
	if (RemainingUnits == 0) return;
	
	const float Fill = static_cast<float>(RemainingUnits) / static_cast<float>(MaxUnitsAvailable);
	FillStaticMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, Fill));
}

// Called every frame
void AFlipperUnitProvider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFlipperUnitProvider::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlipperUnitProvider, RemainingUnits);
}

void AFlipperUnitProvider::Interact_Implementation(AActor* Interactor)
{
	if (!HasAuthority()) return;
	if (RemainingUnits == 0) return;
	if (ProvidedFinish == FGameplayTag::EmptyTag) return;
	
	UFlipperUsableUnitComponent* UnitsComponent = UFlipperUsableUnitComponent::FindUnitComponentOfType(Interactor, UnitType);
	if (!UnitsComponent) return;

	// update the current finish on the units component to whatever this unit provider sets
	// do this before no unit remaining check, so that even with, for instance, a full paint roller, the 'equipped' finish can be changed
	UnitsComponent->SetCurrentFinish(ProvidedFinish);
	
	const uint8 FreeSpace = UnitsComponent->GetMaxUsableUnits() - UnitsComponent->GetUsableUnits();
	uint8 UnitsToGive = FMath::Min(FreeSpace, RemainingUnits);
	if (UnitsToGive == 0) return; // if there's no units left to give or the interactor units are full already
	
	if (UnitsComponent->TryAddUnit(UnitsToGive))
	{
		RemainingUnits -= UnitsToGive;
		OnRep_RemainingUnits(); // trigger a host update of the remaining units
	}
}

