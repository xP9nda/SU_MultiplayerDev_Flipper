// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlipperGameMode.h"

#include "FlipperGameState.h"
#include "Interactables/Instances/Standard/FlipperJobSite.h"

AFlipperGameMode::AFlipperGameMode()
{
	// stub
}

void AFlipperGameMode::RegisterJobSite(AFlipperJobSite* JobSite)
{
	if (!JobSite) return;
	
	JobSite->OnJobSiteCompleted.BindDynamic(this, &AFlipperGameMode::HandleJobSiteCompleted);
	
	JobSites.Add(JobSite);
}

void AFlipperGameMode::HandleJobSiteCompleted(const AFlipperJobSite* JobSite) const
{
	if (!JobSite) return;
	if (JobSite->GetJobState() != EFlipperJobState::Completed) return;

	const UWorld* World = GetWorld();
	if (!World) return;
	
	AFlipperGameState* GS = World->GetGameState<AFlipperGameState>();
	if (!GS) return;
	
	GS->AddFunds(JobSite->GetCompletionPayout());
}

void AFlipperGameMode::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	if (!World) return;
	
	AFlipperGameState* GS = World->GetGameState<AFlipperGameState>();
	if (!GS) return;
	
	GS->AddFunds(StartingMoney);
}
