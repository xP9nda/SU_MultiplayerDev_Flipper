// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperGameState.h"

#include "FlipperGameMode.h"
#include "FlipperPlayerState.h"
#include "Interactables/Instances/Standard/FlipperJobSite.h"
#include "Net/UnrealNetwork.h"

void AFlipperGameState::AddFunds(int32 Amount, AFlipperPlayerState* Earner)
{
	if (!HasAuthority()) return;
	if (Amount <= 0) return;
	
	TeamFunds += Amount;
	OnRep_TeamFunds();
	
	if (Earner)
	{
		Earner->AddFundsEarned(Amount);
	}
}

void AFlipperGameState::RefundFunds(int32 Amount, AFlipperPlayerState* Spender)
{
	if (!HasAuthority()) return;
	if (Amount <= 0) return;
	
	TeamFunds += Amount;
	OnRep_TeamFunds();
	
	// this was specifically a refund, and as such, the player's individual contribution to funds spent must be reverted
	if (Spender)
	{
		Spender->RemoveFundsSpent(Amount);
	}
}

bool AFlipperGameState::TrySpendFunds(int32 Amount, AFlipperPlayerState* Spender)
{
	if (!HasAuthority()) return false;
	if (Amount < 0) return false;
	if (Amount > TeamFunds) return false;
	
	TeamFunds -= Amount;
	OnRep_TeamFunds();
	
	if (Spender)
	{
		Spender->AddFundsSpent(Amount);
	}
	
	return true;
}

void AFlipperGameState::RegisterJobSite(AFlipperJobSite* JobSite)
{
	// no authority guard here as the list of job sites exists on all clients independently for hooking into UI and such
	if (JobSites.Contains(JobSite)) return; // don't register this if it's already for some reason been registered
	
	JobSites.Add(JobSite);
	// broadcast that the job site has been registered locally so UI can pick it up
	OnJobSiteRegistered.Broadcast(JobSite);
	
	// if we have authority, register the callback to the game mode's handle job completed for completion payout
	if (HasAuthority())
	{
		const UWorld* World = GetWorld();
		if (!World) return;
		
		const AFlipperGameMode* FlipperGameMode = World->GetAuthGameMode<AFlipperGameMode>();
		if (!FlipperGameMode) return;
		
		JobSite->OnJobSiteCompleted.AddDynamic(FlipperGameMode, &AFlipperGameMode::HandleJobSiteCompleted);
	}
}

void AFlipperGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlipperGameState, TeamFunds);
}

void AFlipperGameState::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	
	OnMatchEnded.Broadcast();
}

void AFlipperGameState::OnRep_TeamFunds()
{
	OnTeamFundsChanged.Broadcast(TeamFunds);
}
