// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperGameSession.h"

#include "FlipperGameInstance.h"


// Sets default values
AFlipperGameSession::AFlipperGameSession()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFlipperGameSession::BeginPlay()
{
	Super::BeginPlay();
	
	const UFlipperGameInstance* GameInstance = GetGameInstance<UFlipperGameInstance>();
	if (GameInstance)
	{
		MaxPlayers = GameInstance->SessionMaxPublicConnections;
	}
	else
	{
		// if it fails just set a default
		MaxPlayers = 4;
	}
	
}


