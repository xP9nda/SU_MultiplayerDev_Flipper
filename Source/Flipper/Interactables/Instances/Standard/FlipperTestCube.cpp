// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperTestCube.h"

#include "Net/UnrealNetwork.h"


// Sets default values
AFlipperTestCube::AFlipperTestCube()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
}

// Called when the game starts or when spawned
void AFlipperTestCube::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFlipperTestCube::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlipperTestCube, bIsInteracted) // one line per property
}


void AFlipperTestCube::OnRep_IsInteracted()
{
	StaticMeshComponent->SetMaterial(0, InteractedMaterial);
}

// Called every frame
void AFlipperTestCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFlipperTestCube::Interact_Implementation(AActor* Interactor)
{
	if (bIsInteracted) return;
	
	bIsInteracted = true; // trigger a state update, which will call OnRep_IsInteracted
	OnRep_IsInteracted(); // call immediately for host to react also
}


