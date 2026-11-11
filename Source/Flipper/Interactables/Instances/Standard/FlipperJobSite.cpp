// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperJobSite.h"

#include "Core/FlipperGameMode.h"
#include "Interactables/Instances/Units/FlipperPaintableMesh.h"
#include "Interactables/Instances/Units/FlipperUnitPickupMesh.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFlipperJobSite::AFlipperJobSite()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AFlipperJobSite::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlipperJobSite, JobState);
	DOREPLIFETIME(AFlipperJobSite, JobProgress);
	
	// these values will never be updated after they're set, so COND_InitialOnly "will only attempt to send on the initial bunch", i.e. initial client update
	// the condition should be removed if these will ever be changed at runtime, but they won't, just making a note of it
	// DOREPLIFETIME_CONDITION(AFlipperJobSite, NumberPaintableMeshes, COND_InitialOnly);
	// DOREPLIFETIME_CONDITION(AFlipperJobSite, NumberPickupMeshes, COND_InitialOnly);
	// commented out as the values can instead be derived in beginplay on clients as well as the server
}

void AFlipperJobSite::Interact_Implementation(AActor* Interactor)
{
	if (!HasAuthority()) return;
	if (JobState != EFlipperJobState::NotStarted) return;
	
	SetJobState(EFlipperJobState::InProgress);
	OnJobSiteStateChanged.ExecuteIfBound(JobState);
}

FFlipperInteractText AFlipperJobSite::GetInteractText(AActor* Interactor)
{
	FFlipperInteractText NewInteractText;
	TArray<FText> DescriptionLines;
	
	switch (JobState)
	{
		case EFlipperJobState::NotStarted:
			NewInteractText.MainText = FText::Format(FText::FromString("Start Job '{0}'"), JobName);
			DescriptionLines.Add(FText::FromString("Click to start the job\n"));
			break;
			
		case EFlipperJobState::InProgress:
			NewInteractText.MainText = FText::Format(FText::FromString("Job '{0}' In Progress"), JobName);
			break;
			
		case EFlipperJobState::Completed:
			NewInteractText.MainText = FText::Format(FText::FromString("Job '{0}' Completed"), JobName);
			break;
	}

	const int32 TotalCompletionSteps = NumberPaintableMeshes + NumberPickupMeshes;
	const int32 CompletedJobSteps = JobProgress.CompletedPaintableMeshes + JobProgress.CompletedPickupMeshes;
	
	const int32 TotalProgress = TotalCompletionSteps > 0 ? FMath::RoundToInt(100.0f * CompletedJobSteps / TotalCompletionSteps) : 0;
	DescriptionLines.Add(FText::Format(FText::FromString("{0}% Complete:"), TotalProgress));
	
	if (NumberPaintableMeshes > 0)
	{
		const int32 Progress = FMath::RoundToInt(100.0f * JobProgress.CompletedPaintableMeshes / NumberPaintableMeshes);
		DescriptionLines.Add(FText::Format(FText::FromString("  - {0}/{1} painted ({2}%)"), JobProgress.CompletedPaintableMeshes, NumberPaintableMeshes, Progress));
	}
	
	if (NumberPickupMeshes > 0)
	{
		const int32 Progress = FMath::RoundToInt(100.0f * JobProgress.CompletedPickupMeshes / NumberPickupMeshes);
		DescriptionLines.Add(FText::Format(FText::FromString("  - {0}/{1} trash picked up ({2}%)"), JobProgress.CompletedPickupMeshes, NumberPickupMeshes, Progress));
	}
	
	const FCulturePtr USCulture = FInternationalization::Get().GetCulture(TEXT("en-US")); // strip the "US" from "US$x.xx"
	const FText FormattedCompletionPayout = FText::AsCurrencyBase(CompletionPayout, TEXT("USD"), USCulture);
	DescriptionLines.Add(FText::Format(FText::FromString("\n({0} completion bonus)"), FormattedCompletionPayout));
	
	NewInteractText.DescriptionText = FText::Join(FText::FromString("\n"), DescriptionLines);
	
	return NewInteractText;
}

void AFlipperJobSite::SetJobState(EFlipperJobState NewJobState)
{
	if (!HasAuthority()) return;
	
	JobState = NewJobState;
	
	// update whether the interactable objects can be interacted with - whether the job is in progress
	SetInteractablesCanInteract(JobState == EFlipperJobState::InProgress);
	
	OnJobSiteStateChanged.ExecuteIfBound(JobState);
	
	if (JobState == EFlipperJobState::Completed)
	{
		OnJobSiteCompleted.ExecuteIfBound(this);
	}
}

void AFlipperJobSite::UpdateCompletionStatus()
{
	if (!HasAuthority()) return; // should only be called from server paths, but to be safe, guard this anyway
	if (JobState == EFlipperJobState::Completed) return;
	
	if (JobProgress.CompletedPaintableMeshes == NumberPaintableMeshes && JobProgress.CompletedPickupMeshes == NumberPickupMeshes)
	{
		SetJobState(EFlipperJobState::Completed);
		OnJobSiteStateChanged.ExecuteIfBound(JobState);
	}
}

void AFlipperJobSite::HandlePaintableMeshCompleted(AFlipperPaintableMesh* Mesh, APawn* FinishedBy)
{
	if (!HasAuthority()) return;
	if (JobState == EFlipperJobState::Completed) return;
	
	JobProgress.CompletedPaintableMeshes++;
	UpdateCompletionStatus();
}

void AFlipperJobSite::HandlePickupMeshCompleted(AFlipperUnitPickupMesh* Mesh, APawn* FinishedBy)
{
	if (!HasAuthority()) return;
	if (JobState == EFlipperJobState::Completed) return;
	
	JobProgress.CompletedPickupMeshes++;
	UpdateCompletionStatus();
}

void AFlipperJobSite::SetInteractablesCanInteract(const bool bNewCanInteract)
{
	for (TObjectPtr<AFlipperPaintableMesh> Mesh : PaintableMeshes)
	{
		if (!IsValid(Mesh)) continue;
		Mesh->SetCanInteract(bNewCanInteract);
	}

	for (TObjectPtr<AFlipperUnitPickupMesh> Mesh : PickupMeshes)
	{
		if (!IsValid(Mesh)) continue;
		Mesh->SetCanInteract(bNewCanInteract);
	}
}

// Called when the game starts or when spawned
void AFlipperJobSite::BeginPlay()
{
	Super::BeginPlay();

	const bool bHasAuthority = HasAuthority();
	
	// only register the delegates for when things are completed on the server,
	// but track the number of paintables and pickups on all clients, so that
	// the values don't need to be replicated as they can be derived
	for (TObjectPtr<AFlipperPaintableMesh> Mesh : PaintableMeshes)
	{
		if (!IsValid(Mesh)) continue;
		
		if (bHasAuthority) Mesh->OnPaintableMeshCompleted.AddUniqueDynamic(this, &AFlipperJobSite::HandlePaintableMeshCompleted);
		NumberPaintableMeshes++;
	}

	for (TObjectPtr<AFlipperUnitPickupMesh> Mesh : PickupMeshes)
	{
		if (!IsValid(Mesh)) continue;
		
		if (bHasAuthority) Mesh->OnPickupMeshCompleted.AddUniqueDynamic(this, &AFlipperJobSite::HandlePickupMeshCompleted);
		NumberPickupMeshes++;
	}
	
	// mark interactable objects as unable to be interacted with, then register the job site only on the server
	if (!bHasAuthority) return;
	
	SetInteractablesCanInteract(false);
	GetWorld()->GetAuthGameMode<AFlipperGameMode>()->RegisterJobSite(this);
	
}