// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperUnitsBaseInteractable.h"


// Sets default values
AFlipperUnitsBaseInteractable::AFlipperUnitsBaseInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AFlipperUnitsBaseInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlipperUnitsBaseInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

