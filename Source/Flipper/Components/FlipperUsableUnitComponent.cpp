// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipperUsableUnitComponent.h"

#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UFlipperUsableUnitComponent::UFlipperUsableUnitComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UFlipperUsableUnitComponent::OnRep_UsableUnits() const
{
	OnUsableUnitsChanged.Broadcast();
}

void UFlipperUsableUnitComponent::OnRep_CurrentFinish() const
{
	OnCurrentFinishChanged.Broadcast();
}

bool UFlipperUsableUnitComponent::TryConsumeUnit(uint8 Quantity)
{
	if (!GetOwner()->HasAuthority()) return false; // only the server can use units
	if (Quantity == 0) return false;
	if (UsableUnits < Quantity) return false; // if there aren't enough units to use
	
	UsableUnits -= Quantity;
	OnRep_UsableUnits(); // immediately trigger a listen server host update
	return true;
}

bool UFlipperUsableUnitComponent::TryAddUnit(uint8 Quantity)
{
	if (!GetOwner()->HasAuthority()) return false;
	if (Quantity == 0) return false;
	if (UsableUnits + Quantity > MaxUsableUnits) return false; // don't allow overflowing the max units

	UsableUnits += Quantity;
	OnRep_UsableUnits();
	return true;
}

void UFlipperUsableUnitComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (DefaultFinish.IsValid())
	{
		CurrentFinish = DefaultFinish;
	}
}

void UFlipperUsableUnitComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// owner only for these values as they're values that are changed by the server and have an authoritative server copy,
	// but other clients don't need these values, only the owner of the pawn that this component is attached to needs to see these values (for ui)
	DOREPLIFETIME_CONDITION(UFlipperUsableUnitComponent, UsableUnits, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UFlipperUsableUnitComponent, CurrentFinish, COND_OwnerOnly);
}

bool UFlipperUsableUnitComponent::TrySetCurrentFinish(const FGameplayTag NewCurrentFinish)
{
	if (!GetOwner()->HasAuthority()) return false;
	if (!NewCurrentFinish.IsValid()) return false; // don't allow holding units with no finish
	
	// don't allow holding units of a different finish while holding units of one already
	if (UsableUnits > 0 && NewCurrentFinish != CurrentFinish) return false;
	
	// if it's the same finish, then user is buying more units of what's already equipped
	// still a successful request, but nothing is changed so no OnRep needs to happen; no unnecessary UI/client updates should occur
	if (CurrentFinish != NewCurrentFinish)
	{
		CurrentFinish = NewCurrentFinish;
		OnRep_CurrentFinish();
	}

	return true;
}

UFlipperUsableUnitComponent* UFlipperUsableUnitComponent::FindUnitComponentOfType(const AActor* Actor, const FGameplayTag& InTag)
{
	if (!IsValid(Actor) || !InTag.IsValid()) return nullptr;

	// store the actor components that are of type UFlipperUsableUnitComponent
	TInlineComponentArray<UFlipperUsableUnitComponent*> ActorComponents;
	Actor->GetComponents(ActorComponents);
	
	for (UFlipperUsableUnitComponent* Component : ActorComponents)
	{
		if (Component->UnitType == InTag) return Component;
	}
	
	return nullptr;
}

