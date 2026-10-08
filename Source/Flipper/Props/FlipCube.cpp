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
	
	if (DynamicMaterial) return;
	DynamicMaterial = UMaterialInstanceDynamic::Create(CubeMaterial, this);
	MeshComponent->SetMaterial(0, DynamicMaterial);
}

// Called every frame
void AFlipCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// if this is a copy on a client then exit out so that the rotation is only applied from the server
	if (!HasAuthority()) return;
	
	AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
	
	TimeSinceLastColour += DeltaTime;
	if (TimeSinceLastColour > TimeBetweenColourChanges)
	{
		// authoritative at this point
		if (Colours.Num() == 0) return;
		
		MaterialColour = Colours[FMath::RandRange(0, Colours.Num() - 1)];
		OnRep_MaterialColour();
		
		TimeSinceLastColour = 0.0f;
	}
}


void AFlipCube::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlipCube, MaterialColour);
}


void AFlipCube::OnRep_MaterialColour()
{
	if (!DynamicMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(CubeMaterial, this);
		MeshComponent->SetMaterial(0, DynamicMaterial);
	}
	
	DynamicMaterial->SetVectorParameterValue("Colour", MaterialColour);
}


