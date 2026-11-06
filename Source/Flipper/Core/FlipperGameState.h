// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FlipperGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamFundsChanged, int32, NewFunds);

UCLASS()
class FLIPPER_API AFlipperGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	void AddFunds(int32 Amount);
	bool TrySpendFunds(int32 Amount);
	
	int32 GetTeamFunds() const { return TeamFunds; }
	
	FOnTeamFundsChanged OnTeamFundsChanged;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
private:
	// team funds is a shared state so it lives on the game state as every client has it,
	// will replicate to everyone and therefore show on all client's HUDs
	UPROPERTY(ReplicatedUsing = OnRep_TeamFunds)
	int32 TeamFunds = 0;
	
	UFUNCTION()
	void OnRep_TeamFunds();
	
};

