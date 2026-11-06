// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperPlayerState.h"

#include "Net/UnrealNetwork.h"

void AFlipperPlayerState::AddSurfaceCompleted()
{
	if (!HasAuthority()) return;
	
	SurfacesCompleted++;
	OnRep_ContributionChanged();
}

void AFlipperPlayerState::AddFundsEarned(int32 Amount)
{
	if (!HasAuthority()) return;
	if (Amount <= 0) return;
	
	FundsEarned += Amount;
	OnRep_ContributionChanged();
}

void AFlipperPlayerState::AddFundsSpent(int32 Amount)
{
	if (!HasAuthority()) return;
	if (Amount <= 0) return;
	
	FundsSpent += Amount;
	OnRep_ContributionChanged();
}

void AFlipperPlayerState::RemoveFundsSpent(int32 Amount)
{
	if (!HasAuthority()) return;
	if (Amount <= 0) return;
	if (Amount > FundsSpent) return;
	
	// clamp the funds spent at 0 just to be safe
	FundsSpent = FMath::Max(FundsSpent - Amount, 0);
	OnRep_ContributionChanged();
}

void AFlipperPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlipperPlayerState, SurfacesCompleted);
	DOREPLIFETIME(AFlipperPlayerState, FundsEarned);
	DOREPLIFETIME(AFlipperPlayerState, FundsSpent);
}

void AFlipperPlayerState::OnRep_ContributionChanged()
{
	OnContributionChanged.Broadcast();
}
