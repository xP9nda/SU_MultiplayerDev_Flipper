// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlipperGameMode.h"

#include "FlipperGameState.h"
#include "Interactables/Instances/Standard/FlipperJobSite.h"

AFlipperGameMode::AFlipperGameMode()
{
	// stub
}

void AFlipperGameMode::HandleJobSiteCompleted(AFlipperJobSite* JobSite) const
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
