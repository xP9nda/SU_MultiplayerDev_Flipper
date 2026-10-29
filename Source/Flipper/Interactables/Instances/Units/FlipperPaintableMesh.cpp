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
	
	const UFlipperSurfaceFinishData* SurfaceData = GetSurfaceData();
	const FFlipperSurfaceFinish* StartSurfaceFinish = SurfaceData ? SurfaceData->GetFinish(StartFinish) : nullptr;
	
	if (StartSurfaceFinish)
	{
		// the starting look of the paintable mesh comes entirely from the start finish's colour and texture
		DesiredMaterialDynamic->SetVectorParameterValue(MaterialStartColourParameterName, StartSurfaceFinish->SurfaceColour);
		
		UTexture* StartTexture = SurfaceData->GetMaterialTexture(StartSurfaceFinish->MaterialTexture);
		if (StartTexture)
		{
			DesiredMaterialDynamic->SetTextureParameterValue(MaterialStartTextureParameterName, StartTexture);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No texture found for %s (start finish %s) on: %s"), *StartSurfaceFinish->MaterialTexture.ToString(), *StartFinish.ToString(), *this->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("StartFinish is missing or not in SurfaceData on: %s"), *this->GetName());
	}
	
	// ensure a late joiner receives updated visuals of the paintable mesh
	OnRep_PaintState();
}

const UFlipperSurfaceFinishData* AFlipperPaintableMesh::GetSurfaceData() const
{
	const UFlipperGameInstance* GameInstance = Cast<UFlipperGameInstance>(GetWorld()->GetGameInstance());
	return GameInstance ? GameInstance->SurfaceData : nullptr;
}

void AFlipperPaintableMesh::OnRep_PaintState()
{
	if (MaxPaintableSteps == 0) return;
	if (!DesiredMaterialDynamic) return;
	
	// always apply progress, so an unpainted wall shows the start finish regardless of the material's default progress
	const float PaintProgress = static_cast<float>(CurrentPaintableStep) / static_cast<float>(MaxPaintableSteps);
	DesiredMaterialDynamic->SetScalarParameterValue(MaterialProgressParameterName, PaintProgress);
	
	if (!CurrentFinish.IsValid()) return;
	
	const UFlipperSurfaceFinishData* SurfaceData = GetSurfaceData();
	const FFlipperSurfaceFinish* CurrentSurfaceFinish = SurfaceData ? SurfaceData->GetFinish(CurrentFinish) : nullptr;
	
	if (CurrentSurfaceFinish)
	{
		// always set the target colour, texture finishes are set as white so the texture ends up not tinted
		DesiredMaterialDynamic->SetVectorParameterValue(MaterialTargetColourParameterName, CurrentSurfaceFinish->SurfaceColour);
		
		FGameplayTag TargetTextureTag = CurrentSurfaceFinish->MaterialTexture;
		if (!TargetTextureTag.IsValid())
		{
			// if target material texture is not valid, replace the texture on the wall with the start finish's texture
			const FFlipperSurfaceFinish* StartSurfaceFinish = SurfaceData->GetFinish(StartFinish);
			if (StartSurfaceFinish)
			{
				TargetTextureTag = StartSurfaceFinish->MaterialTexture;
			}
		}
		
		UTexture* TargetTexture = SurfaceData->GetMaterialTexture(TargetTextureTag);
		if (TargetTexture)
		{
			DesiredMaterialDynamic->SetTextureParameterValue(MaterialTargetTextureParameterName, TargetTexture);
		}
	}
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
	const FGameplayTag HeldFinish = UnitsComponent->GetCurrentFinish();
	if (!HeldFinish.IsValid()) return;
	
	const UFlipperSurfaceFinishData* SurfaceData = GetSurfaceData();
	if (!SurfaceData) return;
	
	// the held paint decides what this wall becomes, e.g. red paint on a wall that wants red brick becomes red brick
	CurrentFinish = SurfaceData->ResolveAppliedFinish(HeldFinish, TargetFinish);
	
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

