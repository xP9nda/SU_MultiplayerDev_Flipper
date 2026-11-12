// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperPlayerState.h"

#include "FFlipperStepsContribution.h"
#include "Net/UnrealNetwork.h"


void AFlipperPlayerState::AddSurfaceContribution(const EFlipperContributionType ContributionType)
{
	if (!HasAuthority()) return;
	
	switch (ContributionType)
	{
		case EFlipperContributionType::Correct:
			SurfaceContribution.CorrectSteps++;
			break;
		
		case EFlipperContributionType::Incorrect:
			SurfaceContribution.IncorrectSteps++;
			break;
		
		default:
			break;
	}
	
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

void AFlipperPlayerState::IncrementPickedUpCount(const FGameplayTag UnitType)
{
	if (!HasAuthority()) return;
	
	// find and add a new picked up count if one for the provided unit type does not exist
	bool bFound = false;
	for (auto Element : PickedUpCounts)
	{
		if (Element.UnitType.MatchesTagExact(UnitType))
		{
			bFound = true;
			break;
		}
	}
	
	if (!bFound)
	{
		FFlipperPickedUpCount NewPickedUpCount;
		NewPickedUpCount.UnitType = UnitType;
		PickedUpCounts.Add(NewPickedUpCount);
	}
	
	// run through all picked up counts and increment the one with the matching unit type
	for (auto& Element : PickedUpCounts)
	{
		if (Element.UnitType.MatchesTagExact(UnitType))
		{
			Element.Amount++;
			OnRep_ContributionChanged();
			return; // matching unit type was found so end the increment
		}
	}
}

FFlipperPickedUpCount AFlipperPlayerState::GetPickedUpCountForUnitType(const FGameplayTag UnitType)
{
	for (auto& Element : PickedUpCounts)
	{
		if (Element.UnitType.MatchesTagExact(UnitType))
		{
			return Element;
		}
	}
	
	return FFlipperPickedUpCount();
}

void AFlipperPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlipperPlayerState, SurfaceContribution);
	DOREPLIFETIME(AFlipperPlayerState, FundsEarned);
	DOREPLIFETIME(AFlipperPlayerState, FundsSpent);
}

void AFlipperPlayerState::OnRep_ContributionChanged()
{
	OnContributionChanged.Broadcast();
}

void AFlipperPlayerState::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);
	
	// retrieve the saved inactive player state values if the player rejoins
	// PostLogin checks for an inactive PlayerState (user left mid-gameplay), if one is found, this OverrideWith is called
	const AFlipperPlayerState* FlipperPlayerState = Cast<AFlipperPlayerState>(PlayerState);
	if (!FlipperPlayerState) return;
	
	SurfaceContribution = FlipperPlayerState->SurfaceContribution;
	FundsEarned = FlipperPlayerState->FundsEarned;
	FundsSpent = FlipperPlayerState->FundsSpent;
	
	// ensure the set values are immediately reflected
	OnRep_ContributionChanged();
}

void AFlipperPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	
	AFlipperPlayerState* FlipperPlayerState = Cast<AFlipperPlayerState>(PlayerState);
	if (!FlipperPlayerState) return;
		
	// copy this player state's properties into the player state provided
	FlipperPlayerState->SurfaceContribution = SurfaceContribution;
	FlipperPlayerState->FundsEarned = FundsEarned;
	FlipperPlayerState->FundsSpent = FundsSpent;
}
