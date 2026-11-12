// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FFlipperStepsContribution.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerState.h"
#include "FlipperPlayerState.generated.h"

struct FGameplayTag;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContributionChanged);

USTRUCT()
struct FFlipperPickedUpCount
{
	GENERATED_BODY()
	
	UPROPERTY()
	FGameplayTag UnitType;
	
	UPROPERTY()
	int32 Amount = 0;
};

UCLASS()
class FLIPPER_API AFlipperPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	// server only through auth guard
	void AddSurfaceContribution(const EFlipperContributionType ContributionType);
	void AddFundsEarned(int32 Amount);
	void AddFundsSpent(int32 Amount);
	void RemoveFundsSpent(int32 Amount);
	void IncrementPickedUpCount(FGameplayTag UnitType);
	
	const FFlipperStepsContribution& GetSurfaceContribution() const { return SurfaceContribution; }
	int32 GetFundsEarned() const { return FundsEarned; }
	int32 GetFundsSpent() const { return FundsSpent; }
	TArray<FFlipperPickedUpCount> GetPickedUpCounts() const { return PickedUpCounts; }
	FFlipperPickedUpCount GetPickedUpCountForUnitType(FGameplayTag UnitType);
	
	FOnContributionChanged OnContributionChanged;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// use the same OnRep function as the scoreboard only needs to know "something changed"
	UPROPERTY(ReplicatedUsing = OnRep_ContributionChanged)
	FFlipperStepsContribution SurfaceContribution;
	
	UPROPERTY(ReplicatedUsing = OnRep_ContributionChanged)
	int32 FundsEarned = 0;
	
	UPROPERTY(ReplicatedUsing = OnRep_ContributionChanged)
	int32 FundsSpent = 0;
	
	UPROPERTY(ReplicatedUsing = OnRep_ContributionChanged)
	TArray<FFlipperPickedUpCount> PickedUpCounts;
	
	UFUNCTION()
	void OnRep_ContributionChanged();

protected:
	virtual void OverrideWith(APlayerState* PlayerState) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
};
