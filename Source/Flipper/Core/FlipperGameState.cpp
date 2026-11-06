// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperGameState.h"

#include "Net/UnrealNetwork.h"

void AFlipperGameState::AddFunds(int32 Amount)
{
	if (!HasAuthority()) return;
	if (Amount <= 0) return;
	
	TeamFunds += Amount;
	OnRep_TeamFunds();
}

bool AFlipperGameState::TrySpendFunds(int32 Amount)
{
	if (!HasAuthority()) return false;
	if (Amount < 0) return false;
	if (Amount > TeamFunds) return false;
	
	TeamFunds -= Amount;
	OnRep_TeamFunds();
	return true;
}

void AFlipperGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlipperGameState, TeamFunds);
}

void AFlipperGameState::OnRep_TeamFunds()
{
	OnTeamFundsChanged.Broadcast(TeamFunds);
}