// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlipperGameMode.h"

#include "FlipperGameState.h"

AFlipperGameMode::AFlipperGameMode()
{
	// stub
}

void AFlipperGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	if (!World) return;
	
	AFlipperGameState* GS = World->GetGameState<AFlipperGameState>();
	if (!GS) return;
	
	GS->AddFunds(StartingMoney);
}
