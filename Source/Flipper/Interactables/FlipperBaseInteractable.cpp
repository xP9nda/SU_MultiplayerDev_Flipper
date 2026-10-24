// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperBaseInteractable.h"

#include "Components/FlipperOutlineComponent.h"


// Sets default values
AFlipperBaseInteractable::AFlipperBaseInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	OutlineComponent = CreateDefaultSubobject<UFlipperOutlineComponent>(TEXT("OutlineComponent"));
}

// Called when the game starts or when spawned
void AFlipperBaseInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlipperBaseInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFlipperBaseInteractable::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("Base interactable method fired."));
}

