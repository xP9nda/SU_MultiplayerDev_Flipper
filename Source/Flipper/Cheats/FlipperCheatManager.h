// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "FlipperCheatManager.generated.h"

/**
 * Cheat manager to test replication for things like funds
 */
UCLASS()
class FLIPPER_API UFlipperCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Exec) // Exec makes it runnable via unreal console
	void FlipperAddFunds(int32 Amount);
	
	UFUNCTION(Exec)
	void FlipperSetNetDebuggingEnabled(bool IsEnabled);
	
	bool IsNetDebuggingEnabled() const { return bIsNetDebuggingEnabled; }
	
private:
	bool bIsNetDebuggingEnabled = false;
};
