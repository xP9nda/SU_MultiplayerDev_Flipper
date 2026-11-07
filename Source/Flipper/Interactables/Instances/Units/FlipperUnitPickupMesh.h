// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Instances/FlipperUnitsBaseInteractable.h"
#include "FlipperUnitPickupMesh.generated.h"

UCLASS()
class FLIPPER_API AFlipperUnitPickupMesh : public AFlipperUnitsBaseInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlipperUnitPickupMesh();
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	// The number of units that will be given when this pickup mesh is picked up
	UPROPERTY(EditAnywhere, Category = "Flipper|Config")
	uint8 UnitsToGive = 1;
	
	UPROPERTY(ReplicatedUsing = OnRep_PickedUp)
	bool bIsPickedUp = false;
	
	UFUNCTION()
	void OnRep_PickedUp();

public:
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FFlipperInteractText GetInteractText(AActor* Interactor) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
