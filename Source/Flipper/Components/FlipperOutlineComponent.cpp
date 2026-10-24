// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperOutlineComponent.h"


// Sets default values for this component's properties
UFlipperOutlineComponent::UFlipperOutlineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UFlipperOutlineComponent::BeginPlay()
{
	Super::BeginPlay();

	// store the owner's static meshes
	// this is so that every time the outline should be shown or hidden, the meshes aren't constantly re-retrieved
	GetOwner()->GetComponents<UStaticMeshComponent>(OwnerStaticMeshes);
}


// Called every frame
void UFlipperOutlineComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFlipperOutlineComponent::ShowOutline()
{
	for (UStaticMeshComponent* Mesh : OwnerStaticMeshes)
	{
		Mesh->SetRenderCustomDepth(true);
	}
}

void UFlipperOutlineComponent::HideOutline()
{
	for (UStaticMeshComponent* Mesh : OwnerStaticMeshes)
	{
		Mesh->SetRenderCustomDepth(false);
	}
}

