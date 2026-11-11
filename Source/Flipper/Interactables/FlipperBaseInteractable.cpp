// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperBaseInteractable.h"

#include "Components/FlipperOutlineComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFlipperBaseInteractable::AFlipperBaseInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	OutlineComponent = CreateDefaultSubobject<UFlipperOutlineComponent>(TEXT("OutlineComponent"));
}

void AFlipperBaseInteractable::SetCanInteract(const bool NewCanInteract)
{
	if (!HasAuthority()) return;
	if (bCanInteract == NewCanInteract) return;
	
	bCanInteract = NewCanInteract;
}

// Called when the game starts or when spawned
void AFlipperBaseInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFlipperBaseInteractable::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("Base interactable method fired."));
}

FFlipperInteractText AFlipperBaseInteractable::GetInteractText(AActor* Interactor)
{
	FFlipperInteractText NewInteractText = FFlipperInteractText();
	NewInteractText.MainText = FText::FromString("Interact");
	NewInteractText.DescriptionText = FText();
	
	return NewInteractText;
}

void AFlipperBaseInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlipperBaseInteractable, bCanInteract);
}

bool AFlipperBaseInteractable::GetCanInteract() const
{
	return bCanInteract;
}