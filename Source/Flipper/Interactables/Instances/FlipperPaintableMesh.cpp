// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperPaintableMesh.h"

#include "Net/UnrealNetwork.h"


// Sets default values
AFlipperPaintableMesh::AFlipperPaintableMesh()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
}

void AFlipperPaintableMesh::SetupMaterial()
{
	if (DesiredMaterial)
	{
		DesiredMaterialDynamic = UMaterialInstanceDynamic::Create(DesiredMaterial, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DesiredMaterial is null on: %s"), *this->GetName());
	}
	
	if (StaticMeshComponent && DesiredMaterialDynamic)
	{
		StaticMeshComponent->SetMaterial(0, DesiredMaterialDynamic);
	}
}

// Called when the game starts or when spawned
void AFlipperPaintableMesh::BeginPlay()
{
	Super::BeginPlay();
	
	SetupMaterial();
	// ensure default material colours are applied
	OnRep_CurrentPaintableStep();
}

void AFlipperPaintableMesh::OnRep_CurrentPaintableStep()
{
	if (MaxPaintableSteps == 0) return;
	if (!DesiredMaterialDynamic) return;
	
	// cast paint progress to flat as uint8 division only returns an integer
	const float PaintProgress = static_cast<float>(CurrentPaintableStep) / static_cast<float>(MaxPaintableSteps);
	DesiredMaterialDynamic->SetScalarParameterValue(MaterialProgressParameterName, PaintProgress);
}

// Called every frame
void AFlipperPaintableMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFlipperPaintableMesh::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	// setup material on construction so it shows default state in level viewport
	SetupMaterial();
}

void AFlipperPaintableMesh::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlipperPaintableMesh, CurrentPaintableStep);
}

void AFlipperPaintableMesh::Interact_Implementation()
{
	if (CurrentPaintableStep >= MaxPaintableSteps) return;
	
	CurrentPaintableStep++;
	OnRep_CurrentPaintableStep();
}

