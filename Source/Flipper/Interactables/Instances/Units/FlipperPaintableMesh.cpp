// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperPaintableMesh.h"

#include "Components/FlipperUsableUnitComponent.h"
#include "Core/FlipperPlayerState.h"
#include "DataAssets/FlipperSurfaceFinishData.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFlipperPaintableMesh::AFlipperPaintableMesh()
{
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

bool AFlipperPaintableMesh::IsCompleteAndCorrect() const
{
	return (CurrentPaintableStep >= MaxPaintableSteps) && (CurrentFinish.MatchesTagExact(TargetFinish));
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
	if (IsCompleteAndCorrect()) return; // don't allow re-painting once the wall is complete and correct
	
	// get the units component for the relevant unit type
	UFlipperUsableUnitComponent* UnitsComponent = GetActiveUnitsComponent(Interactor);
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
	const FGameplayTag NewFinish = SurfaceData->ResolveAppliedFinish(HeldFinish, TargetFinish);
	
	// if the new finish differs from the current finish and the steps are above 0, reset paint progress
	if (!NewFinish.MatchesTagExact(CurrentFinish) && CurrentPaintableStep > 0)
	{
		CurrentPaintableStep = 0;
	}
	
	CurrentFinish = NewFinish;
	
	// check the current paintable step AND unit consume at once,
	// so that a completed wall can still be re-finished, but an incomplete wall will use a unit and advance progress
	// 
	// step check condition has to come first so that completed walls don't consume units, logic fails at the AND condition and doesn't progress
	if (CurrentPaintableStep < MaxPaintableSteps && UnitsComponent->TryConsumeUnit(1))
	{
		CurrentPaintableStep++;
		
		// only if a unit was taken, track contribution
		AFlipperPlayerState* InteractorPlayerState = GetInteractorPlayerState(Interactor);
		if (InteractorPlayerState)
		{
			if (CurrentFinish.MatchesTagExact(TargetFinish))
			{
				StepsPerPlayer.FindOrAdd(InteractorPlayerState).CorrectSteps++; // increment the CORRECT steps that this interactor has contributed to the wall
				InteractorPlayerState->AddSurfaceContribution(EFlipperContributionType::Correct);
			}
			else
			{
				StepsPerPlayer.FindOrAdd(InteractorPlayerState).IncorrectSteps++; // increment the INCORRECT steps that this interactor has contributed to the wall 
				InteractorPlayerState->AddSurfaceContribution(EFlipperContributionType::Incorrect);
			}
		}
	}
	
	if (IsCompleteAndCorrect())
	{
		OnPaintableMeshCompleted.Broadcast(this, Cast<APawn>(Interactor));
	}
	
	OnRep_PaintState(); // listen server host update
}

FFlipperInteractText AFlipperPaintableMesh::GetInteractText(AActor* Interactor)
{
	FFlipperInteractText NewInteractText = FFlipperInteractText();
	
	if (IsCompleteAndCorrect())
	{
		NewInteractText.MainText = FText::FromString("Surface Finished");
		NewInteractText.DescriptionText = FText::FromString("100% done, with correct type"); // can presume 100% done rather than work it out mathematically
		return NewInteractText;
	}
	
	const UFlipperSurfaceFinishData* SurfaceData = GetSurfaceData();
	const UFlipperUsableUnitComponent* UnitsComponent = GetActiveUnitsComponent(Interactor);
	if (!SurfaceData || !UnitsComponent)
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
	
	const FFlipperSurfaceFinish* InteractorCurrentFinishData = SurfaceData->GetFinish(UnitsComponent->GetCurrentFinish());
	if (!InteractorCurrentFinishData)
	{
		// if the interactor has no finish held, assume no units either, so prompt them to purchase units at the shop
		NewInteractText.MainText = FText::FromString("No type is held");
	}
	else
	{
		// interactor has a valid finish, so show that it can be applied
		NewInteractText.MainText = FText::Format(FText::FromString("Apply {0}"), InteractorCurrentFinishData->DisplayName);
	}
	
	const FFlipperSurfaceFinish* TargetSurfaceFinishData = SurfaceData->GetFinish(TargetFinish);
	if (!TargetSurfaceFinishData)
	{
		NewInteractText.DescriptionText = FText::FromString("An error occured fetching information for this object");
	}
	else
	{
		// target surface finish data is valid
		FText TargetFinishDisplayName = TargetSurfaceFinishData->DisplayName;
		
		// if the target finish is applied by paint, show the paint that finishes it instead
		if (TargetSurfaceFinishData->AppliedByPaint.IsValid())
		{
			const FFlipperSurfaceFinish* AppliedBySurfaceFinish = SurfaceData->GetFinish(TargetSurfaceFinishData->AppliedByPaint);
			// if there's an error finding the applied by surface finish, just show "unknown"
			TargetFinishDisplayName = AppliedBySurfaceFinish ? AppliedBySurfaceFinish->DisplayName : FText::FromString("unknown");
		}
		
		NewInteractText.DescriptionText = FText::Format(
			FText::FromString("{0}% done, with {1} type. Wants {2}"),
			(static_cast<float>(CurrentPaintableStep) / static_cast<float>(MaxPaintableSteps)) * 100,
			CurrentFinish.MatchesTagExact(TargetFinish) ? FText::FromString("correct") : FText::FromString("incorrect"), 
			TargetFinishDisplayName
		);
	}
	
	return NewInteractText;
}

