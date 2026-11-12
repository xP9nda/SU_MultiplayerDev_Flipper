// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlipperGameMode.h"

#include "FlipperGameState.h"
#include "Interactables/Instances/Standard/FlipperJobSite.h"

AFlipperGameMode::AFlipperGameMode()
{
	// stub
}

void AFlipperGameMode::HandleJobSiteCompleted(AFlipperJobSite* JobSite)
{
	if (!JobSite) return;
	if (JobSite->GetJobState() != EFlipperJobState::Completed) return; // if the job site isn't yet completed, return

	const UWorld* World = GetWorld();
	if (!World) return;
	
	AFlipperGameState* FlipperGameState = World->GetGameState<AFlipperGameState>();
	if (!FlipperGameState) return;
	
	FlipperGameState->AddFunds(JobSite->GetCompletionPayout());
	
	// run through all the job sites and check if everything is completed
	for (const TWeakObjectPtr<AFlipperJobSite>& SitePointer : FlipperGameState->GetJobSites())
	{
		const AFlipperJobSite* Site = SitePointer.Get();
		if (!Site) return;
		if (Site->GetJobState() != EFlipperJobState::Completed) return; 
	}
	
	// at this point all job sites are completed, so call EndMatch
	// AGameMode moves to MatchState::WaitingPostMatch which replicates in the game state
	EndMatch();
}

void AFlipperGameMode::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	if (!World) return;
	
	AFlipperGameState* FlipperGameState = World->GetGameState<AFlipperGameState>();
	if (!FlipperGameState) return;
	
	FlipperGameState->AddFunds(StartingMoney);
}

void AFlipperGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AFlipperGameMode::Logout(AController* Exiting)
{
	// when the pawn is destroyed the usable unit components on them are lost too, meaning their held units are lost.
	// could refund the team or such with whatever the player had upon leaving, but it's already been paid for so not going to do that
	
	Super::Logout(Exiting);
}
