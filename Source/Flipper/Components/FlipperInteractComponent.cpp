// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperInteractComponent.h"

#include "Interfaces/FlipperInteract.h"


// Sets default values for this component's properties
UFlipperInteractComponent::UFlipperInteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	
	// ...
}


// Called when the game starts
void UFlipperInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFlipperInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FHitResult UFlipperInteractComponent::PerformLinetrace() // todo - document
{
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner()); // ignore the owner (player char) from collision
	
	FVector StartLocation;
	FRotator StartRotation;
	GetOwner()->GetActorEyesViewPoint(StartLocation, StartRotation);
	
	GetWorld()->LineTraceSingleByChannel(
		Hit,
		StartLocation,
		(StartRotation.Vector() * InteractionDistance) + StartLocation,
		ECC_WorldStatic,
		CollisionParams
	);
	
	DrawDebugLine(GetWorld(),
		StartLocation,
(StartRotation.Vector() * InteractionDistance) + StartLocation,
		FColor::Red,
		false,
		2.0f
	);
	
	return Hit;
}


// -------- Client
void UFlipperInteractComponent::AttemptInteract()
{
	// check if the client is within interaction cooldown; prevent unnecessary calls to server
	double CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime < LocalAllowedInteractionTime) return;
	
	const FHitResult Hit = PerformLinetrace();
	if (!Hit.bBlockingHit) return; // nothing was hit
	if (!Hit.GetActor()) return; // hit actor is invalid
	if (!Hit.GetActor()->Implements<UFlipperInteract>()) return; // if hit actor doesn't implement interact
	
	LocalAllowedInteractionTime = CurrentTime + InteractionCooldown;
	Server_AttemptInteraction();
}

// -------- Server
void UFlipperInteractComponent::Server_AttemptInteraction_Implementation()
{
	// check whether the interaction is allowed right now
	double CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime < AllowedInteractionTime) return; // don't allow the interaction, it's too soon
	
	// re-validate the linetrace on the server to ensure client isn't lying
	const FHitResult Hit = PerformLinetrace();
	if (!Hit.bBlockingHit) return; // nothing was hit
	if (!Hit.GetActor()) return; // hit actor is invalid
	if (!Hit.GetActor()->Implements<UFlipperInteract>()) return; // if hit actor doesn't implement interact
	
	AllowedInteractionTime = CurrentTime + (InteractionCooldown * 0.9f); // allow for a bit of network jitter
	
	// do interaction this way to avoid a null pointer on the cast, in case the hit actor doesn't implement interaction
	// send through this component's owner from the server as the user who interacted
	IFlipperInteract::Execute_Interact(Hit.GetActor(), GetOwner()); 
	
	// Cast<IFlipperInteract>(Hit.GetActor())->Execute_Interact(Hit.GetActor()); // cast to interact interface to perform interact method
}

bool UFlipperInteractComponent::Server_AttemptInteraction_Validate()
{
	return true;
}


