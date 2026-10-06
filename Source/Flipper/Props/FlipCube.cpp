// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipCube.h"


// Sets default values
AFlipCube::AFlipCube()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	SetReplicateMovement(true);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	
	SetRootComponent(MeshComponent);
}

// Called when the game starts or when spawned
void AFlipCube::BeginPlay()
{
	Super::BeginPlay();
	MeshComponent->SetStaticMesh(Mesh);
	
}

// Called every frame
void AFlipCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// if this is a copy on a client then exit out so that the rotation is only applied from the server
	if (!HasAuthority()) return;
	
	AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
}

