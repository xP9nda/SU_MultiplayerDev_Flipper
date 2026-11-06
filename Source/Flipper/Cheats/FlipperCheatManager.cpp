// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperCheatManager.h"

#include "FlipperPlayerController.h"

void UFlipperCheatManager::FlipperAddFunds(int32 Amount)
{
	AFlipperPlayerController* PlayerController = Cast<AFlipperPlayerController>(GetOuterAPlayerController());
	if (!PlayerController) return;
	
	PlayerController->Server_Cheat_AddFunds(Amount);
}

void UFlipperCheatManager::FlipperSetNetDebuggingEnabled(bool IsEnabled)
{
	bIsNetDebuggingEnabled = IsEnabled;
}
