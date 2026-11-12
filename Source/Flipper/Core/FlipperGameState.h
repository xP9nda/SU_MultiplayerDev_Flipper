// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FlipperGameState.generated.h"

class AFlipperJobSite;
class AFlipperPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamFundsChanged, int32, NewFunds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJobSiteRegistered, AFlipperJobSite*, JobSite);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchEnded);

UCLASS()
class FLIPPER_API AFlipperGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	void AddFunds(int32 Amount, AFlipperPlayerState* Earner = nullptr);
	void RefundFunds(int32 Amount, AFlipperPlayerState* Spender = nullptr);
	bool TrySpendFunds(int32 Amount, AFlipperPlayerState* Spender = nullptr);
	int32 GetTeamFunds() const { return TeamFunds; }
	
	void RegisterJobSite(AFlipperJobSite* JobSite);
	TArray<TWeakObjectPtr<AFlipperJobSite>> GetJobSites() { return JobSites; }
	
	FOnTeamFundsChanged OnTeamFundsChanged;
	FOnJobSiteRegistered OnJobSiteRegistered;
	FOnMatchEnded OnMatchEnded;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void HandleMatchHasEnded() override;
	
private:
	// team funds is a shared state so it lives on the game state as every client has it,
	// will replicate to everyone and therefore show on all client's HUDs
	UPROPERTY(ReplicatedUsing = OnRep_TeamFunds)
	int32 TeamFunds = 0;
	
	UFUNCTION()
	void OnRep_TeamFunds();

private:
	// job sites exists on game state so that each client gets a copy of it locally and can react to changes
	// don't need this to be in the game mode as the completion function can be bound to from World->GetAuthGameMode() on authority
	TArray<TWeakObjectPtr<AFlipperJobSite>> JobSites;
};

