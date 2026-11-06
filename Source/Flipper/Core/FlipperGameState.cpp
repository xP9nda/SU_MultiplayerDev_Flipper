// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperGameState.h"

#include "FlipperPlayerState.h"
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

void AFlipperGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlipperGameState, TeamFunds);
}

void AFlipperGameState::OnRep_TeamFunds()
{
	OnTeamFundsChanged.Broadcast(TeamFunds);
}