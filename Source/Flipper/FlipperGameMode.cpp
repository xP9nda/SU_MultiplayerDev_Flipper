// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlipperGameMode.h"

#include "Props/FlipCube.h"

AFlipperGameMode::AFlipperGameMode()
{
	// stub
}

void AFlipperGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (!CubePropClass) return;
	GetWorld()->SpawnActor<AFlipCube>(
		CubePropClass, FVector(0.0f, 0.0f, 200.0f), FRotator::ZeroRotator
	);
	GetWorld()->SpawnActor<AFlipCube>(
		CubePropClass, FVector(0.0f, 0.0f, 300.0f), FRotator::ZeroRotator
	);
	GetWorld()->SpawnActor<AFlipCube>(
		CubePropClass, FVector(0.0f, 0.0f, 400.0f), FRotator::ZeroRotator
	);
}
