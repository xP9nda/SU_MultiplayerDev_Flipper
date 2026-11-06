// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FlipperPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContributionChanged);

UCLASS()
class FLIPPER_API AFlipperPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	// server only through auth guard
	void AddSurfaceCompleted();
	void AddFundsEarned(int32 Amount);
	
	int32 GetSurfacesCompleted() const { return SurfacesCompleted; }
	int32 GetFundsEarned() const { return FundsEarned; }
	
	FOnContributionChanged OnContributionChanged;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// use the same OnRep function as the scoreboard only needs to know "something changed"
	UPROPERTY(ReplicatedUsing = OnRep_ContributionChanged)
	int32 SurfacesCompleted = 0;
	
	UPROPERTY(ReplicatedUsing = OnRep_ContributionChanged)
	int32 FundsEarned = 0;
	
	UFUNCTION()
	void OnRep_ContributionChanged();

};
