// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperPaintableMesh.h"

#include "FlipperGameInstance.h"
#include "Components/FlipperUsableUnitComponent.h"
#include "DataAssets/FlipperSurfaceFinishData.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFlipperPaintableMesh::AFlipperPaintableMesh()
{
	bReplicates = true;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
}

void AFlipperPaintableMesh::SetupMaterial()
{
	if (DesiredMaterial)
	{
		DesiredMaterialDynamic = UMaterialInstanceDynamic::Create(DesiredMaterial, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DesiredMaterial is null on: %s"), *this->GetName());
	}
	
	if (StaticMeshComponent && DesiredMaterialDynamic)
	{
		StaticMeshComponent->SetMaterial(0, DesiredMaterialDynamic);
	}
}

// Called when the game starts or when spawned
void AFlipperPaintableMesh::BeginPlay()
{
	Super::BeginPlay();
	
	SetupMaterial();
	
	// don't continue if no dynamic material at this point
	if (!DesiredMaterialDynamic) return;
	
	// ensure default material parameters are applied
	const UFlipperGameInstance* GameInstance = Cast<UFlipperGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance && GameInstance->SurfaceData)
	{
		// only continue if the start finish is valid
		if (StartFinish != FGameplayTag::EmptyTag)
		{
			// get the starting surface finish from the game instance data asset and ensure the material is set up with that information
			const FFlipperSurfaceFinish* StartSurfaceFinish = GameInstance->SurfaceData->GetFinish(StartFinish);
			if (StartSurfaceFinish)
			{
				// update the start colour and texture with whatever surface finish is set to be the current one
				DesiredMaterialDynamic->SetVectorParameterValue(MaterialStartColourParameterName, StartSurfaceFinish->SurfaceColour);
				DesiredMaterialDynamic->SetTextureParameterValue(MaterialStartTextureParameterName, StartSurfaceFinish->SurfaceTexture);
				
				// @note addition in favour of walls adopting their starting finish texture all the way through
				// such that walls never change texture while being painted
				// for instance, painting green paint onto a brick texture should not override a brick texture with a plaster one
				// 
				// if the surface texture won't be applied to target, then also set the target as the starting finish's surface texture
				if (!StartSurfaceFinish->SurfaceTextureAppliedAsTarget)
				{
					DesiredMaterialDynamic->SetTextureParameterValue(MaterialTargetTextureParameterName, StartSurfaceFinish->SurfaceTexture);
				}
			}
		}
	}
	
	// ensure a late joiner receives an update visual of the paintable mesh
	OnRep_PaintState();
}

void AFlipperPaintableMesh::OnRep_PaintState()
{
	if (MaxPaintableSteps == 0) return;
	if (!DesiredMaterialDynamic) return;
	if (CurrentFinish == FGameplayTag::EmptyTag) return;
	
	// get the game instance
	const UFlipperGameInstance* GameInstance = Cast<UFlipperGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance && GameInstance->SurfaceData)
	{
		// get the currently being painted surface finish from the game instance data asset
		const FFlipperSurfaceFinish* TargetSurfaceFinish = GameInstance->SurfaceData->GetFinish(CurrentFinish);
		if (TargetSurfaceFinish)
		{
			// update the target colour and texture with whatever surface finish the player has 'equipped'
			DesiredMaterialDynamic->SetVectorParameterValue(MaterialTargetColourParameterName, TargetSurfaceFinish->SurfaceColour);
			
			// @note redaction in favour of walls adopting their starting finish texture
			// only apply the surface texture to the target parameter if the paintable mesh should adopt the target finish's texture after painting
			if (TargetSurfaceFinish->SurfaceTextureAppliedAsTarget)
			{
				DesiredMaterialDynamic->SetTextureParameterValue(MaterialTargetTextureParameterName, TargetSurfaceFinish->SurfaceTexture); 
			}
		}
	}
	
	const float PaintProgress = static_cast<float>(CurrentPaintableStep) / static_cast<float>(MaxPaintableSteps);
	DesiredMaterialDynamic->SetScalarParameterValue(MaterialProgressParameterName, PaintProgress);
}

// Called every frame
void AFlipperPaintableMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFlipperPaintableMesh::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlipperPaintableMesh, CurrentPaintableStep);
	DOREPLIFETIME(AFlipperPaintableMesh, CurrentFinish);
}

void AFlipperPaintableMesh::Interact_Implementation(AActor* Interactor)
{
	if (!HasAuthority()) return;
	
	// get the units component for the relevant unit type
	UFlipperUsableUnitComponent* UnitsComponent = UFlipperUsableUnitComponent::FindUnitComponentOfType(Interactor, UnitType);
	if (!UnitsComponent) return;

	// if the user has no usable units to use, cancel
	// this has to happen before the wall finish is applied below to ensure that players with no units can't paint over a wall's finish
	if (UnitsComponent->GetUsableUnits() == 0) return;
	
	// don't allow players with no finish to visually wipe a wall's progress
	if (UnitsComponent->GetCurrentFinish() == FGameplayTag::EmptyTag) return;
	
	CurrentFinish = UnitsComponent->GetCurrentFinish();
	
	// check the current paintable step AND unit consume at once,
	// so that a completed wall can still be re-finished, but an incomplete wall will use a unit and advance progress
	// 
	// step check condition has to come first so that completed walls don't consume units, logic fails at the AND condition and doesn't progress
	if (CurrentPaintableStep < MaxPaintableSteps && UnitsComponent->TryConsumeUnit(1))
	{
		CurrentPaintableStep++;
	}
	
	OnRep_PaintState(); // listen server host update
}

